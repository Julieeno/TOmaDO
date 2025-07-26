#include "timer/compacttimerwidget.h"
#include "database/databasemanager.h"
#include "settings/settingsdialog.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QMessageBox>
#include <QDebug>
#include <QFont>
#include <QSizePolicy>
#include <QApplication>
#include <QMainWindow>
#include <QKeyEvent>
#include <QScreen>
#include <QTextEdit>
#include <QPlainTextEdit>

CompactTimerWidget::CompactTimerWidget(QWidget* parent)
    : QWidget(parent)
    , isExpanded(false)
    , compactWidget(nullptr)
    , expandedWidget(nullptr)
    , mainLayout(nullptr)
    , m_stackedWidget(nullptr)
    , m_centralWidget(nullptr)
    , m_originalMainLayout(nullptr)
    , compactTimeLabel(nullptr)
    , compactTaskLabel(nullptr)
    , expandedTimeLabel(nullptr)
    , descriptionEdit(nullptr)
    , workTimeSpinBox(nullptr)
    , breakTimeSpinBox(nullptr)
    , taskTitleLabel(nullptr)
    , goalSelectionCombo(nullptr)
    , todayTimeSpentLabel(nullptr)
    , goalHeaderLabel(nullptr)
    , m_transitionAnimation(nullptr)
    , m_transitionProgress(0.0)
    , m_isTransitioning(false)
    , m_timerArea(nullptr)
    , m_lavaLampWidget(nullptr)
{
    // Initialize managers
    m_timerCore = std::make_unique<TimerCore>(this);
    m_buttonManager = std::make_unique<TimerButtonManager>(this);
    m_goalsManager = std::make_unique<TimerGoalsManager>(this);
    m_displayManager = std::make_unique<TimerDisplayManager>(this);

    // Setup UI and connections
    initializeCompactMode();
    connectTimerCore();
    connectGoalsManager();
}

void CompactTimerWidget::onTimerStateChanged(TimerCore::State newState) {
    updateButtonStates();
    updateLavaLampColors();
    updateDisplay();
}

void CompactTimerWidget::onTimerTimeUpdated(int remainingSeconds) {
    m_displayManager->updateTimeDisplays(remainingSeconds);
}

void CompactTimerWidget::onWorkSessionCompleted() {
    SoundManager::instance().playTimerEndSound();

    updateLavaLampColors();
    updateButtonStates();
    updateDisplay();
}

void CompactTimerWidget::onBreakSessionCompleted() {
    SoundManager::instance().playBreakEndSound();

    updateDisplay();
    updateButtonStates();
    updateGoalHeader();
    updateLavaLampColors();

    update();
}

void CompactTimerWidget::onTimerReset() {
    m_displayManager->clearDisplays();
    updateButtonStates();
    updateGoalHeader();
}

void CompactTimerWidget::setupCompactUi() {
    compactWidget = new QWidget(this);
    if (compactWidget) {
        compactWidget->setStyleSheet(
            "QWidget {"
            "   background: transparent;"
            "   border-radius: 14px;"
            "}"
        );
        compactWidget->setObjectName("compactTimerWidget");
    }

    QHBoxLayout* compactLayout = new QHBoxLayout(compactWidget);
    compactLayout->setContentsMargins(16, 12, 16, 12);
    compactLayout->setSpacing(12);

    setupLavaLampBackground();

    QPushButton* expandButton = m_buttonManager->createButton(TimerButtonManager::ButtonType::Expand, TimerButtonManager::ViewMode::Compact);
    expandButton->setToolTip("Expand Timer");
    expandButton->setParent(compactWidget);
    expandButton->hide();
    m_buttonManager->connectButtonSignals(TimerButtonManager::ButtonType::Expand, TimerButtonManager::ViewMode::Compact, this, SLOT(expandTimer()));

    QSettings settings("TOmaDO", "Settings");
    int durationMinutes = settings.value("timer_duration", 25).toInt();
    QString timeText = QString("%1:00").arg(durationMinutes, 2, 10, QChar('0'));

    compactTimeLabel = new QLabel(timeText, compactWidget);
    compactTimeLabel->setAlignment(Qt::AlignCenter);
    compactTimeLabel->setStyleSheet(
        "QLabel {"
        "   font-size: 30px;"
        "   font-weight: 600;"
        "   color: white;"
        "   background-color: transparent;"
        "   border: none;"
        "}"
    );

    QPushButton* compactStartButton = m_buttonManager->createButton(TimerButtonManager::ButtonType::Start, TimerButtonManager::ViewMode::Compact);
    QPushButton* compactPauseButton = m_buttonManager->createButton(TimerButtonManager::ButtonType::Pause, TimerButtonManager::ViewMode::Compact);
    QPushButton* compactEndBreakButton = m_buttonManager->createButton(TimerButtonManager::ButtonType::EndBreak, TimerButtonManager::ViewMode::Compact);

    m_buttonManager->connectButtonSignals(TimerButtonManager::ButtonType::Start, TimerButtonManager::ViewMode::Compact, this, SLOT(startTimer()));
    m_buttonManager->connectButtonSignals(TimerButtonManager::ButtonType::Pause, TimerButtonManager::ViewMode::Compact, this, SLOT(pauseTimer()));
    m_buttonManager->connectButtonSignals(TimerButtonManager::ButtonType::EndBreak, TimerButtonManager::ViewMode::Compact, this, SLOT(endBreakTimer()));

    compactPauseButton->hide();
    compactEndBreakButton->hide();

    compactLayout->addStretch(1);
    compactLayout->addWidget(compactTimeLabel, 0, Qt::AlignCenter);
    compactLayout->addStretch(1);
    compactLayout->addWidget(compactStartButton, 0, Qt::AlignRight | Qt::AlignVCenter);
    compactLayout->addWidget(compactPauseButton, 0, Qt::AlignRight | Qt::AlignVCenter);
    compactLayout->addWidget(compactEndBreakButton, 0, Qt::AlignRight | Qt::AlignVCenter);

    compactWidget->setFixedSize(270, 72);
    mainLayout->addWidget(compactWidget, 0, Qt::AlignCenter);

    // Configure display manager with UI components
    m_displayManager->setUIComponents(
        compactTimeLabel,
        nullptr,  // compactTaskLabel will be set when created
        nullptr,  // expandedTimeLabel will be set when setupExpandedUi is called
        nullptr   // taskTitleLabel will be set when setupExpandedUi is called
    );

    updateDisplay();
    updateLavaLampColors();
}

void CompactTimerWidget::toggleTimer() {
    if (!m_timerCore) return;

    TimerCore::State currentState = m_timerCore->currentState();
    bool isBreakTime = m_timerCore->isBreakTime();

    if (isBreakTime && currentState == TimerCore::State::Break) {
        endBreakTimer();
    } else if (currentState == TimerCore::State::Idle) {
        startTimer();
    } else if (currentState == TimerCore::State::Running || currentState == TimerCore::State::Break) {
        pauseTimer();
    } else if (currentState == TimerCore::State::Paused) {
        m_timerCore->startTimer();
    }
}

void CompactTimerWidget::startTimer() {
    if (m_timerCore->isRunning()) return;

    int workTime = SettingsDialog::getDefaultTimerDuration();
    if (workTimeSpinBox) {
        workTime = workTimeSpinBox->value();
    }
    m_timerCore->setWorkTime(workTime);

    m_timerCore->startTimer();
    updateButtonStates();
    updateDisplay();
    updateLavaLampColors();
}

void CompactTimerWidget::pauseTimer() {
    if (!m_timerCore) return;

    if (m_timerCore->currentState() == TimerCore::State::Paused) {
        m_timerCore->startTimer();
    } else {
        m_timerCore->pauseTimer();
    }
}

void CompactTimerWidget::stopTimer() {
    if (m_timerCore->currentState() == TimerCore::State::Break) {
        finishBreakTimer();
    } else {
        m_timerCore->stopTimer();
    }
}

void CompactTimerWidget::updateButtonStates() {
    if (!m_timerCore || !m_buttonManager) return;

    bool isRunning = m_timerCore->isRunning();
    bool isPaused = m_timerCore->isPaused();
    bool isBreakTime = m_timerCore->currentState() == TimerCore::State::Break;

    m_buttonManager->updateButtonStates(isRunning, isPaused, isExpanded, isBreakTime);
}

void CompactTimerWidget::updateLavaLampColors() {
    if (!m_lavaLampWidget) {
        return;
    }

    if (m_timerCore->currentState() == TimerCore::State::Break || m_timerCore->isBreakTime()) {
        // Break time - use blue/purple colors
        QColor coolPrimary(64, 120, 192, 240);    // Blue
        QColor coolSecondary(128, 64, 192, 220);  // Purple
        QColor coolBackground(0, 75, 130);        // Dark blue background

        m_lavaLampWidget->setColorScheme(coolPrimary, coolSecondary, coolBackground);
        m_lavaLampWidget->setTemperatureBasedColors(true);
    } else {
        // Work time - use red/orange colors
        QColor warmPrimary(255, 60, 20, 240);     // Red-orange
        QColor warmSecondary(255, 140, 0, 220);   // Orange
        QColor warmBackground(130, 75, 0);         // Dark background

        m_lavaLampWidget->setColorScheme(warmPrimary, warmSecondary, warmBackground);
        m_lavaLampWidget->setTemperatureBasedColors(false);
    }
}

void CompactTimerWidget::updateDisplay() {
    // Update time displays
    m_displayManager->updateTimeDisplays(m_timerCore->remainingSeconds());

    // Update task displays
    m_displayManager->updateTaskDisplays(
        m_timerCore->currentGoalId(),
        m_timerCore->currentTitle(),
        m_timerCore->currentDescription()
    );

    // Update goal header
    m_goalsManager->updateGoalHeader(
        m_timerCore->currentGoalId(),
        m_timerCore->currentTitle()
    );

    // Update today's time spent
    m_goalsManager->updateTodayTimeSpent();
}

void CompactTimerWidget::startBreakTimer() {
    m_timerCore->startBreakTimer();
    updateLavaLampColors();
    updateButtonStates();
    updateDisplay();
}

void CompactTimerWidget::finishBreakTimer() {
    m_timerCore->finishBreakTimer();

    updateDisplay();
    updateButtonStates();
    updateGoalHeader();
    updateLavaLampColors();

    update();
}


void CompactTimerWidget::connectTimerCore() {
    connect(m_timerCore.get(), &TimerCore::stateChanged, this, &CompactTimerWidget::onTimerStateChanged);
    connect(m_timerCore.get(), &TimerCore::timeUpdated, this, &CompactTimerWidget::onTimerTimeUpdated);
    connect(m_timerCore.get(), &TimerCore::workSessionCompleted, this, &CompactTimerWidget::onWorkSessionCompleted);
    connect(m_timerCore.get(), &TimerCore::breakSessionCompleted, this, &CompactTimerWidget::onBreakSessionCompleted);
    connect(m_timerCore.get(), &TimerCore::timerReset, this, &CompactTimerWidget::onTimerReset);
    connect(m_timerCore.get(), &TimerCore::sessionDataReady, this, &CompactTimerWidget::onSessionDataReady);
}

void CompactTimerWidget::saveToDatabase(const TimerCore::SessionData& data) {
    if (!data.startTime.isValid() || !data.pauseStartTime.isValid()) {
        return;
    }

    int actualSeconds = data.startTime.secsTo(data.pauseStartTime) - data.totalPausedSeconds;
    if (actualSeconds <= 0) {
        return;
    }

    DatabaseManager::TimerRecord record;
    record.goalId = data.goalId;
    record.startTime = data.startTime;
    record.endTime = data.pauseStartTime;

    DatabaseManager& dbManager = DatabaseManager::instance();
    dbManager.addTimerRecord(record);
}


void CompactTimerWidget::onSessionDataReady(const TimerCore::SessionData& data) {
    saveToDatabase(data);

    // Update UI
    emit databaseUpdated();
    updateTodayTimeSpent();
}

void CompactTimerWidget::enterEvent(QEvent* event) {
    QWidget::enterEvent(event);

    if (!isExpanded) {
        QPushButton* expandBtn = m_buttonManager->getButton(TimerButtonManager::ButtonType::Expand, TimerButtonManager::ViewMode::Compact);
        if (expandBtn) {
            expandBtn->show();
            expandBtn->raise();
        }
    }
}

void CompactTimerWidget::leaveEvent(QEvent* event) {
    QWidget::leaveEvent(event);

    if (!isExpanded) {
        QPushButton* expandBtn = m_buttonManager->getButton(TimerButtonManager::ButtonType::Expand, TimerButtonManager::ViewMode::Compact);
        if (expandBtn) {
            if (!m_timerCore || !m_timerCore->isRunning()) {
                expandBtn->hide();
            }
        }
    }
}

void CompactTimerWidget::setMainWindowComponents(QStackedWidget* stackedWidget, QWidget* centralWidget, QWidget* mainWidget, QHBoxLayout* mainLayout) {
    m_stackedWidget = stackedWidget;
    m_centralWidget = centralWidget;
    m_mainWidget = mainWidget;
    m_mainLayout = mainLayout;
    m_originalMainLayout = qobject_cast<QVBoxLayout*>(centralWidget->layout());

    for (int i = 0; i < mainLayout->count(); ++i) {
        QLayoutItem* item = mainLayout->itemAt(i);
        if (item && item->widget()) {
            QWidget* widget = item->widget();
            if (i == 0 && widget->maximumWidth() < 1000) {
                m_sidebarWidget = widget;
                break;
            }
        }
    }
}

void CompactTimerWidget::initializeCompactMode() {
    if (mainLayout) {
        QLayoutItem* item;
        while ((item = mainLayout->takeAt(0)) != nullptr) {
            delete item;
        }
        delete mainLayout;
        mainLayout = nullptr;
    }

    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    mainLayout->setAlignment(Qt::AlignCenter);

    if (!compactWidget) {
        setupCompactUi();
    }
    if (compactWidget) {
        compactWidget->show();
        mainLayout->addWidget(compactWidget, 0, Qt::AlignCenter);
    }
    if (expandedWidget) {
        expandedWidget->hide();
    }

    setFixedHeight(80);
    setMinimumWidth(200);
    setMaximumWidth(400);

    isExpanded = false;

    updateDisplay();
    updateButtonStates();
}

void CompactTimerWidget::setupLavaLampBackground() {
    m_lavaLampWidget = new LavaLampPaintWidget(this);

    updateLavaLampColors();
    m_lavaLampWidget->setColorScheme(
        QColor(255, 60, 20, 240),  // Primary color (red-ish)
        QColor(255, 140, 0, 220),   // Secondary color (orange-ish)
        QColor(130, 75, 0)       // Background color (dark blue)
    );

    m_lavaLampWidget->setBorderRadius(20);

    m_lavaLampWidget->lower();
    m_lavaLampWidget->startAnimation();
}

void CompactTimerWidget::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);

    if (!isExpanded) {
        QPushButton* expandBtn = m_buttonManager->getButton(TimerButtonManager::ButtonType::Expand, TimerButtonManager::ViewMode::Compact);
        if (expandBtn && compactWidget) {
            // Position the expand button
            expandBtn->move(8, 8);
            expandBtn->show();
            expandBtn->raise();
        }
    }

    if (m_lavaLampWidget) {
        m_lavaLampWidget->resize(size());
    }
    if (compactWidget && !isExpanded) {
        compactWidget->move((width() - compactWidget->width()) / 2,
                           (height() - compactWidget->height()) / 2);
    }
    if (expandedWidget && isExpanded) {
        expandedWidget->resize(size());
    }
}

void CompactTimerWidget::setupExpandedUi() {
    if (expandedWidget) return;

    expandedWidget = new QWidget(this);
    expandedWidget->setMinimumSize(800, 800);

    expandedWidget->setStyleSheet(
        "QWidget {"
        "   background: transparent;"
        "}"
    );

    QVBoxLayout* expandedLayout = new QVBoxLayout(expandedWidget);

    QWidget* headerWidget = new QWidget(expandedWidget);
    QVBoxLayout* headerLayout = new QVBoxLayout(headerWidget);
    headerWidget->setStyleSheet("QWidget { background: transparent; }");
    headerLayout->setContentsMargins(20, 20, 20, 10);
    headerLayout->setSpacing(10);

    QHBoxLayout* topRowLayout = new QHBoxLayout();
    topRowLayout->setSpacing(10);

    goalHeaderLabel = new QLabel("No Goal Selected", headerWidget);
    goalHeaderLabel->setStyleSheet(
        "QLabel {"
        "   font-size: 30px;"
        "   font-weight: bold;"
        "   text-align: left;"
        "   color: white;"
        "   background: transparent;"
        "}"
    );

    QPushButton* collapseButton = m_buttonManager->createButton(TimerButtonManager::ButtonType::Collapse, TimerButtonManager::ViewMode::Expanded);
    m_buttonManager->connectButtonSignals(TimerButtonManager::ButtonType::Collapse, TimerButtonManager::ViewMode::Expanded, this, SLOT(collapseTimer()));

    topRowLayout->addWidget(goalHeaderLabel, 1);
    topRowLayout->addWidget(collapseButton, 0);

    todayTimeSpentLabel = new QLabel("Today: 0h 0m", headerWidget);
    todayTimeSpentLabel->setStyleSheet(
        "QLabel {"
        "   font-size: 16px;"
        "   color: white;"
        "   background: transparent;"
        "}"
    );

    headerLayout->addLayout(topRowLayout);
    headerLayout->addWidget(todayTimeSpentLabel);

    expandedLayout->addWidget(headerWidget);

    QWidget* centreWidget = new QWidget(expandedWidget);
    QVBoxLayout* centreLayout = new QVBoxLayout(centreWidget);
    centreWidget->setStyleSheet("QWidget { background: transparent; }");
    centreLayout->setAlignment(Qt::AlignCenter);
    centreLayout->setSpacing(20);

    // Timer display
    QSettings settings("TOmaDO", "Settings");
    int durationMinutes = settings.value("timer_duration", 25).toInt();
    QString timeText = QString("%1:00").arg(durationMinutes, 2, 10, QChar('0'));

    expandedTimeLabel = new QLabel(timeText, centreWidget);
    expandedTimeLabel->setStyleSheet(
        "QLabel {"
        "   font-size: 150px;"
        "   font-weight: 500;"
        "   color: white;"
        "   padding: 25px;"
        "   letter-spacing: 2px;"
        "   background: transparent;"
        "}"
    );
    expandedTimeLabel->setAlignment(Qt::AlignCenter);
    centreLayout->addWidget(expandedTimeLabel);

    // Task title label (if you need it separate from goalHeaderLabel)
    taskTitleLabel = new QLabel("", centreWidget);
    taskTitleLabel->setStyleSheet(
        "QLabel {"
        "   font-size: 20px;"
        "   font-weight: 400;"
        "   color: white;"
        "   background: transparent;"
        "   margin: 10px 0;"
        "}"
    );
    taskTitleLabel->setAlignment(Qt::AlignCenter);
    taskTitleLabel->setVisible(false); // Hide by default, show when needed
    centreLayout->addWidget(taskTitleLabel);

    QWidget* buttonContainer = new QWidget(centreWidget);
    QHBoxLayout* buttonLayout = new QHBoxLayout(buttonContainer);
    buttonLayout->setAlignment(Qt::AlignCenter);
    buttonLayout->setSpacing(10);

    QPushButton* expandedStartButton = m_buttonManager->createButton(TimerButtonManager::ButtonType::Start, TimerButtonManager::ViewMode::Expanded);
    QPushButton* expandedPauseButton = m_buttonManager->createButton(TimerButtonManager::ButtonType::Pause, TimerButtonManager::ViewMode::Expanded);
    QPushButton* expandedStopButton = m_buttonManager->createButton(TimerButtonManager::ButtonType::Stop, TimerButtonManager::ViewMode::Expanded);
    QPushButton* expandedEndBreakButton = m_buttonManager->createButton(TimerButtonManager::ButtonType::EndBreak, TimerButtonManager::ViewMode::Expanded);

    expandedPauseButton->hide();
    expandedStopButton->hide();
    expandedEndBreakButton->hide();

    buttonLayout->addWidget(expandedStartButton);
    buttonLayout->addWidget(expandedPauseButton);
    buttonLayout->addWidget(expandedStopButton);
    buttonLayout->addWidget(expandedEndBreakButton);

    centreLayout->addWidget(buttonContainer);

    expandedLayout->addStretch(1);
    expandedLayout->addWidget(centreWidget);
    expandedLayout->addStretch(1);

    setupExpandedSettings(expandedLayout);

    m_buttonManager->connectButtonSignals(TimerButtonManager::ButtonType::Start, TimerButtonManager::ViewMode::Expanded, this, SLOT(startTimer()));
    m_buttonManager->connectButtonSignals(TimerButtonManager::ButtonType::Pause, TimerButtonManager::ViewMode::Expanded, this, SLOT(pauseTimer()));
    m_buttonManager->connectButtonSignals(TimerButtonManager::ButtonType::Stop, TimerButtonManager::ViewMode::Expanded, this, SLOT(stopTimer()));
    m_buttonManager->connectButtonSignals(TimerButtonManager::ButtonType::EndBreak, TimerButtonManager::ViewMode::Expanded, this, SLOT(endBreakTimer()));

    mainLayout->addWidget(expandedWidget);
    expandedWidget->hide();

    // Update TimerDisplayManager with all UI components (including expanded ones)
    m_displayManager->setUIComponents(
        compactTimeLabel,        // from compact UI
        compactTaskLabel,        // from compact UI (may be nullptr if not created yet)
        expandedTimeLabel,       // from expanded UI
        taskTitleLabel          // from expanded UI
    );

    // Update TimerGoalsManager with expanded UI components
    m_goalsManager->setUIComponents(goalSelectionCombo, goalHeaderLabel, todayTimeSpentLabel);
    m_goalsManager->populateGoalsComboBox();
    m_goalsManager->updateTodayTimeSpent();

    // Initial display update
    updateDisplay();
}

void CompactTimerWidget::endBreakTimer() {
    m_timerCore->finishBreakTimer();
    updateDisplay();
    updateButtonStates();
}

void CompactTimerWidget::setupExpandedSettings(QVBoxLayout* expandedLayout) {
    QHBoxLayout* goalLayout = new QHBoxLayout();
    QLabel* goalLabel = new QLabel("Goal:", expandedWidget);
    goalLabel->setStyleSheet(
        "QLabel { "
        "   font-size: 14px; "
        "   font-weight: 600; "
        "   color: #8B4B6B; "
        "}"
    );
    goalLayout->addWidget(goalLabel);

    goalSelectionCombo = new QComboBox(expandedWidget);
    if (!goalSelectionCombo) {
        qCritical() << "Failed to create goalSelectionCombo";
        return;
    }

    goalSelectionCombo->setStyleSheet(
        "QComboBox { "
        "   padding: 8px 12px; "
        "   font-size: 14px; "
        "   color: #8B4B6B; "
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "       stop:0 #FFFFFF, stop:1 #FFFAFA);"
        "   border: 2px solid #FFD6D6; "
        "   border-radius: 8px; "
        "}"
        "QComboBox:hover {"
        "   border: 2px solid #FFB6C1;"
        "}"
        "QComboBox::drop-down {"
        "   border: none;"
        "   width: 25px;"
        "}"
        "QComboBox::down-arrow {"
        "   image: none;"
        "   border: 5px solid transparent;"
        "   border-top: 5px solid #8B4B6B;"
        "   margin-right: 8px;"
        "}"
    );
    goalSelectionCombo->setMinimumWidth(200);
    goalLayout->addWidget(goalSelectionCombo);

    goalLayout->addStretch();
    expandedLayout->addLayout(goalLayout);

    descriptionEdit = new QLineEdit(expandedWidget);
    if (!descriptionEdit) {
        qCritical() << "Failed to create descriptionEdit";
        return;
    }

    descriptionEdit->setPlaceholderText("What are you working on? (Leave empty for untitled session)");
    descriptionEdit->setStyleSheet(
        "QLineEdit { "
        "   padding: 12px 16px; "
        "   font-size: 16px; "
        "   color: #8B4B6B; "
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "       stop:0 #FFFFFF, stop:1 #FFFAFA);"
        "   border: 2px solid #FFD6D6; "
        "   border-radius: 8px; "
        "}"
        "QLineEdit:hover {"
        "   border: 2px solid #FFB6C1;"
        "}"
        "QLineEdit:focus {"
        "   border: 2px solid #FF69B4;"
        "   outline: none;"
        "}"
    );
    expandedLayout->addWidget(descriptionEdit);

    QHBoxLayout* settingsLayout = new QHBoxLayout();

    QLabel* workLabel = new QLabel("Work time:", expandedWidget);
    workLabel->setStyleSheet("QLabel { font-size: 14px; font-weight: 600; color: #8B4B6B; }");
    settingsLayout->addWidget(workLabel);

    workTimeSpinBox = new QSpinBox(expandedWidget);
    if (!workTimeSpinBox) {
        qCritical() << "Failed to create workTimeSpinBox";
        return;
    }

    if (workTimeSpinBox) {
        connect(workTimeSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
                m_timerCore.get(), &TimerCore::setWorkTime);
    }
    if (breakTimeSpinBox) {
        connect(breakTimeSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
                m_timerCore.get(), &TimerCore::setBreakTime);
    }

    workTimeSpinBox->setRange(1, 120);
    workTimeSpinBox->setValue(SettingsDialog::getDefaultTimerDuration());
    workTimeSpinBox->setSuffix(" min");
    workTimeSpinBox->setStyleSheet(
        "QSpinBox { "
        "   padding: 8px 12px; "
        "   font-size: 14px; "
        "   color: #8B4B6B; "
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "       stop:0 #FFFFFF, stop:1 #FFFAFA);"
        "   border: 2px solid #FFD6D6; "
        "   border-radius: 8px; "
        "}"
        "QSpinBox:hover {"
        "   border: 2px solid #FFB6C1;"
        "}"
        "QSpinBox::up-button, QSpinBox::down-button {"
        "   background: transparent;"
        "   border: none;"
        "   width: 20px;"
        "}"
        "QSpinBox::up-arrow {"
        "   border: 4px solid transparent;"
        "   border-bottom: 4px solid #8B4B6B;"
        "}"
        "QSpinBox::down-arrow {"
        "   border: 4px solid transparent;"
        "   border-top: 4px solid #8B4B6B;"
        "}"
    );
    settingsLayout->addWidget(workTimeSpinBox);

    QLabel* breakLabel = new QLabel("Break time:", expandedWidget);
    breakLabel->setStyleSheet("QLabel { font-size: 14px; font-weight: 600; color: #8B4B6B; }");
    settingsLayout->addWidget(breakLabel);

    breakTimeSpinBox = new QSpinBox(expandedWidget);
    if (!breakTimeSpinBox) {
        qCritical() << "Failed to create breakTimeSpinBox";
        return;
    }

    breakTimeSpinBox->setRange(1, 30);
    breakTimeSpinBox->setValue(SettingsDialog::getDefaultBreakDuration());
    breakTimeSpinBox->setSuffix(" min");
    breakTimeSpinBox->setStyleSheet(
        "QSpinBox { "
        "   padding: 8px 12px; "
        "   font-size: 14px; "
        "   color: #8B4B6B; "
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "       stop:0 #FFFFFF, stop:1 #FFFAFA);"
        "   border: 2px solid #FFD6D6; "
        "   border-radius: 8px; "
        "}"
        "QSpinBox:hover {"
        "   border: 2px solid #FFB6C1;"
        "}"
        "QSpinBox::up-button, QSpinBox::down-button {"
        "   background: transparent;"
        "   border: none;"
        "   width: 20px;"
        "}"
        "QSpinBox::up-arrow {"
        "   border: 4px solid transparent;"
        "   border-bottom: 4px solid #8B4B6B;"
        "}"
        "QSpinBox::down-arrow {"
        "   border: 4px solid transparent;"
        "   border-top: 4px solid #8B4B6B;"
        "}"
    );
    settingsLayout->addWidget(breakTimeSpinBox);

    settingsLayout->addStretch();
    expandedLayout->addLayout(settingsLayout);

    goalSelectionCombo->hide();
    descriptionEdit->hide();
    workTimeSpinBox->hide();
    breakTimeSpinBox->hide();
    goalLayout->itemAt(0)->widget()->hide();
    settingsLayout->itemAt(0)->widget()->hide();
    settingsLayout->itemAt(2)->widget()->hide();
}

void CompactTimerWidget::refreshGoalsList() {
    m_goalsManager->refreshGoalsList();
}

void CompactTimerWidget::updateTodayTimeSpent() {
    m_goalsManager->updateTodayTimeSpent();
}

void CompactTimerWidget::updateGoalHeader() {
    m_goalsManager->updateGoalHeader(
        m_timerCore->currentGoalId(),
        m_timerCore->currentTitle()
    );
}

void CompactTimerWidget::connectGoalsManager() {
    connect(m_goalsManager.get(), &TimerGoalsManager::goalSelectionChanged,
            this, &CompactTimerWidget::onGoalSelectionChanged);
}

void CompactTimerWidget::onGoalSelectionChanged(int goalId, const QString& title, const QString& description) {
    if (goalId != -1) {
        m_timerCore->startWithGoal(goalId, title, description);

        if (compactTaskLabel) {
            compactTaskLabel->setText(QString("Ready: %1").arg(title));
        }
        if (taskTitleLabel) {
            taskTitleLabel->setText(title);
        }
        if (descriptionEdit) {
            descriptionEdit->setText(description);
        }
    } else {
        if (compactTaskLabel) {
            compactTaskLabel->setText("Ready");
        }
        if (taskTitleLabel) {
            taskTitleLabel->setText("Ready to start");
        }
        if (descriptionEdit) {
            descriptionEdit->clear();
        }
    }
    updateGoalHeader();
}

void CompactTimerWidget::expandTimer() {
    if (isExpanded) {
        return;
    }

    if (!expandedWidget) {
        setupExpandedUi();
    }

    switchToExpandedMode();
    updateDisplay();
    updateButtonStates();
}

void CompactTimerWidget::mousePressEvent(QMouseEvent* event) {
    emit clicked();
    QWidget::mousePressEvent(event);
}

void CompactTimerWidget::startTimerWithGoal(int goalId, const QString& title, const QString& description) {
    m_timerCore->startWithGoal(goalId, title, description);

    if (compactTaskLabel) {
        compactTaskLabel->setText(title);
    } else {
        if (!compactWidget) {
            setupCompactUi();
        }
        if (compactTaskLabel) {
            compactTaskLabel->setText(title);
        }
    }

    if (taskTitleLabel) {
        taskTitleLabel->setText(title);
    } else {
        if (!expandedWidget) {
            setupExpandedUi();
        }
        if (taskTitleLabel) {
            taskTitleLabel->setText(title);
        }
    }

    if (descriptionEdit) {
        if (auto textEdit = qobject_cast<QTextEdit*>(descriptionEdit)) {
            textEdit->setPlainText(description);
        } else if (auto plainTextEdit = qobject_cast<QPlainTextEdit*>(descriptionEdit)) {
            plainTextEdit->setPlainText(description);
        } else if (auto lineEdit = qobject_cast<QLineEdit*>(descriptionEdit)) {
            lineEdit->setText(description);
        }
    }

    updateGoalHeader();
    updateTodayTimeSpent();
}

void CompactTimerWidget::startQuickTimer() {
    m_timerCore->startWithGoal(-1, "Quick Timer", "");

    if (taskTitleLabel) {
        taskTitleLabel->setText("Quick Timer");
    }
    if (descriptionEdit) {
        descriptionEdit->clear();
    }
    if (goalSelectionCombo) {
        goalSelectionCombo->setCurrentIndex(0);
    }
}

bool CompactTimerWidget::shouldTakeLongBreak() const {
    return m_timerCore->shouldTakeLongBreak();
}

void CompactTimerWidget::refreshSettings() {
    QSettings settings("TOmaDO", "Settings");

    int workTime = settings.value("timer_duration", 25).toInt();
    int breakTime = settings.value("break_duration", 5).toInt();
    int longBreakTime = settings.value("long_break_duration", 15).toInt();

    m_timerCore->setWorkTime(workTime);
    m_timerCore->setBreakTime(breakTime);
    m_timerCore->setLongBreakTime(longBreakTime);

    // UI updates
    if (workTimeSpinBox) {
        workTimeSpinBox->setValue(workTime);
    }
    if (breakTimeSpinBox) {
        breakTimeSpinBox->setValue(breakTime);
    }
}

void CompactTimerWidget::setSelectedGoal(int goalId) {
    m_goalsManager->setSelectedGoal(goalId);
}

void CompactTimerWidget::startExpandedTransition() {
    if (!m_mainWidget || !m_mainLayout || !m_originalMainLayout) {
        switchToExpandedMode();
        return;
    }

    m_isTransitioning = true;

    QPoint originalPos = pos();
    QSize originalSize = size();

    m_timerArea = nullptr;
    for (int i = 0; m_originalMainLayout && i < m_originalMainLayout->count(); ++i) {
        QLayoutItem* item = m_originalMainLayout->itemAt(i);
        if (item && item->widget()) {
            QWidget* widget = item->widget();
            if (widget->objectName() == "timerArea" ||
                (widget->maximumHeight() == 100 && widget->layout())) {
                m_timerArea = widget;
                break;
            }
        }
    }

    if (!m_timerArea && m_centralWidget) {
        m_timerArea = new QWidget(m_centralWidget);
        m_timerArea->setObjectName("timerArea");
        m_timerArea->setFixedHeight(100);
        m_timerArea->setStyleSheet(
            "QWidget {"
            "   background-color: #ecf0f1;"
            "   border-top: 2px solid #bdc3c7;"
            "}"
        );
        QHBoxLayout* timerLayout = new QHBoxLayout(m_timerArea);
        timerLayout->setContentsMargins(0, 10, 0, 10);
        timerLayout->setAlignment(Qt::AlignCenter);
        m_originalMainLayout->addWidget(m_timerArea);
    }

    if (!m_timerArea) {
        switchToExpandedMode();
        return;
    }

    QPoint globalPos = mapToGlobal(QPoint(0, 0));

    m_startGeometry = QRect(originalPos, originalSize);

    QMainWindow* mainWindow = qobject_cast<QMainWindow*>(m_mainWidget);
    if (mainWindow) {
        setParent(mainWindow);

        QPoint localPos = mainWindow->mapFromGlobal(globalPos);
        move(localPos);

        m_startGeometry = QRect(localPos, originalSize);
        m_endGeometry = QRect(0, 0, mainWindow->width(), mainWindow->height());
    } else {
        switchToExpandedMode();
        return;
    }

    if (m_transitionAnimation) {
        m_transitionAnimation->stop();
        delete m_transitionAnimation;
        m_transitionAnimation = nullptr;
    }

    m_transitionAnimation = new QPropertyAnimation(this, "transitionProgress");
    m_transitionAnimation->setDuration(TRANSITION_DURATION_MS);
    m_transitionAnimation->setStartValue(0.0);
    m_transitionAnimation->setEndValue(1.0);
    m_transitionAnimation->setEasingCurve(QEasingCurve::InOutQuad);

    connect(m_transitionAnimation, &QPropertyAnimation::finished,
            this, &CompactTimerWidget::onTransitionFinished);

    if (!compactWidget) {
        setupCompactUi();
    }
    if (!expandedWidget) {
        setupExpandedUi();
    }

    if (compactWidget) compactWidget->show();
    if (expandedWidget) expandedWidget->show();

    show();
    raise();
    activateWindow();

    m_transitionAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

void CompactTimerWidget::startCompactTransition() {
    if (!m_mainWidget || !m_mainLayout || !m_originalMainLayout) {
        switchToCompactMode();
        return;
    }

    m_isTransitioning = true;

    m_timerArea = nullptr;
    for (int i = 0; m_originalMainLayout && i < m_originalMainLayout->count(); ++i) {
        QLayoutItem* item = m_originalMainLayout->itemAt(i);
        if (item && item->widget()) {
            QWidget* widget = item->widget();
            if (widget->objectName() == "timerArea" ||
                (widget->maximumHeight() == 100 && widget->layout())) {
                m_timerArea = widget;
                break;
            }
        }
    }

    if (!m_timerArea && m_centralWidget) {
        m_timerArea = new QWidget(m_centralWidget);
        m_timerArea->setObjectName("timerArea");
        m_timerArea->setFixedHeight(100);
        m_timerArea->setStyleSheet(
            "QWidget {"
            "   background-color: #ecf0f1;"
            "   border-top: 2px solid #bdc3c7;"
            "}"
        );
        QHBoxLayout* timerLayout = new QHBoxLayout(m_timerArea);
        timerLayout->setContentsMargins(0, 10, 0, 10);
        timerLayout->setAlignment(Qt::AlignCenter);
        m_originalMainLayout->addWidget(m_timerArea);
    }

    if (!m_timerArea) {
        switchToCompactMode();
        return;
    }

    m_startGeometry = geometry();

    QMainWindow* mainWindow = qobject_cast<QMainWindow*>(m_mainWidget);
    if (mainWindow) {
        setParent(mainWindow);
    } else {
        switchToCompactMode();
        return;
    }

    QPoint timerAreaCenter = m_timerArea->mapToGlobal(
        QPoint(m_timerArea->width() / 2, m_timerArea->height() / 2));

    QPoint compactCenter = mainWindow->mapFromGlobal(timerAreaCenter);

    int compactWidth = 300;
    int compactHeight = 80;
    int compactX = compactCenter.x() - compactWidth / 2;
    int compactY = compactCenter.y() - compactHeight / 2;

    m_endGeometry = QRect(compactX, compactY, compactWidth, compactHeight);

    if (m_transitionAnimation) {
        m_transitionAnimation->stop();
        delete m_transitionAnimation;
        m_transitionAnimation = nullptr;
    }

    m_transitionAnimation = new QPropertyAnimation(this, "transitionProgress");
    m_transitionAnimation->setDuration(TRANSITION_DURATION_MS);
    m_transitionAnimation->setStartValue(0.0);
    m_transitionAnimation->setEndValue(1.0);
    m_transitionAnimation->setEasingCurve(QEasingCurve::InOutQuad);

    connect(m_transitionAnimation, &QPropertyAnimation::finished,
            this, &CompactTimerWidget::onTransitionFinished);

    if (!compactWidget) {
        setupCompactUi();
    }
    if (!expandedWidget) {
        setupExpandedUi();
    }

    if (compactWidget) compactWidget->show();
    if (expandedWidget) expandedWidget->show();

    show();
    raise();
    activateWindow();

    m_transitionAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

void CompactTimerWidget::setWidgetOpacity(QWidget* widget, qreal opacity) {
    if (!widget) return;

    QGraphicsOpacityEffect* effect = qobject_cast<QGraphicsOpacityEffect*>(widget->graphicsEffect());
    if (!effect) {
        effect = new QGraphicsOpacityEffect(widget);
        widget->setGraphicsEffect(effect);
    }

    effect->setOpacity(qBound(0.0, opacity, 1.0));
}

void CompactTimerWidget::setTransitionProgress(qreal progress) {
    m_transitionProgress = qBound(0.0, progress, 1.0);

    if (m_isTransitioning) {
        if (!isExpanded) {
            setStyleSheet(
                "CompactTimerWidget {"
                "   background: transparent;"
                "   border: 2px solid #e9ecef;"
                "   border-radius: 14px;"
                "   margin: 2px;"
                "}"
            );
        }

        int x = m_startGeometry.x() + (m_endGeometry.x() - m_startGeometry.x()) * m_transitionProgress;
        int y = m_startGeometry.y() + (m_endGeometry.y() - m_startGeometry.y()) * m_transitionProgress;
        int width = m_startGeometry.width() + (m_endGeometry.width() - m_startGeometry.width()) * m_transitionProgress;
        int height = m_startGeometry.height() + (m_endGeometry.height() - m_startGeometry.height()) * m_transitionProgress;

        setGeometry(x, y, width, height);

        qreal compactOpacity = isExpanded ? (1.0 - m_transitionProgress) : m_transitionProgress;
        qreal expandedOpacity = isExpanded ? m_transitionProgress : (1.0 - m_transitionProgress);

        if (compactWidget) {
            setWidgetOpacity(compactWidget, compactOpacity);
        }

        if (expandedWidget) {
            setWidgetOpacity(expandedWidget, expandedOpacity);
        }

        raise();
        update();
    }
}

void CompactTimerWidget::onTransitionFinished() {
    m_isTransitioning = false;

    if (m_transitionAnimation) {
        m_transitionAnimation->disconnect();
        m_transitionAnimation = nullptr;
    }

    if (compactWidget) {
        compactWidget->setGraphicsEffect(nullptr);
    }
    if (expandedWidget) {
        expandedWidget->setGraphicsEffect(nullptr);
    }

    if (isExpanded) {
        if (m_sidebarWidget) {
            m_sidebarWidget->hide();
        }
        if (m_centralWidget) {
            m_centralWidget->hide();
        }
        if (m_mainWidget && m_mainLayout) {
            setParent(m_mainWidget);
            m_mainLayout->addWidget(this);
        }

        setMaximumHeight(16777215);
        setMinimumHeight(0);
        setMinimumWidth(0);
        setMaximumWidth(16777215);

        setStyleSheet("");

        if (expandedWidget) expandedWidget->show();
        if (compactWidget) compactWidget->hide();

        updateTodayTimeSpent();
        emit expandedStateChanged(true);
    } else {
        if (m_sidebarWidget) {
            m_sidebarWidget->show();
        }
        if (m_centralWidget) {
            m_centralWidget->show();
        }

        if (m_timerArea) {
            setParent(m_timerArea);
            if (QHBoxLayout* timerLayout = qobject_cast<QHBoxLayout*>(m_timerArea->layout())) {
                timerLayout->addWidget(this, 0, Qt::AlignCenter);
            }
        }

        setFixedHeight(80);
        setMinimumWidth(200);
        setMaximumWidth(400);

        setStyleSheet(
            "CompactTimerWidget {"
            "   background: transparent;"
            "   border: 2px solid #e9ecef;"
            "   border-radius: 14px;"
            "   margin: 2px;"
            "}"
        );

        if (compactWidget) compactWidget->show();
        if (expandedWidget) expandedWidget->hide();

        emit expandedStateChanged(false);
    }

    updateDisplay();
    updateButtonStates();
    show();
}

void CompactTimerWidget::collapseTimer() {
    if (m_transitionAnimation) {
        m_transitionAnimation->stop();
        delete m_transitionAnimation;
        m_transitionAnimation = nullptr;
    }

    m_isTransitioning = false;

    m_timerArea = nullptr;
    for (int i = 0; m_originalMainLayout && i < m_originalMainLayout->count(); ++i) {
        QLayoutItem* item = m_originalMainLayout->itemAt(i);
        if (item && item->widget()) {
            QWidget* widget = item->widget();
            if (widget->objectName() == "timerArea" ||
                (widget->maximumHeight() == 100 && widget->layout())) {
                m_timerArea = widget;
                break;
            }
        }
    }

    if (!m_timerArea && m_centralWidget) {
        m_timerArea = new QWidget(m_centralWidget);
        m_timerArea->setObjectName("timerArea");
        m_timerArea->setFixedHeight(100);
        m_timerArea->setStyleSheet(
            "QWidget {"
            "   background-color: #ecf0f1;"
            "   border-top: 2px solid #bdc3c7;"
            "}"
        );
        QHBoxLayout* timerLayout = new QHBoxLayout(m_timerArea);
        timerLayout->setContentsMargins(0, 10, 0, 10);
        timerLayout->setAlignment(Qt::AlignCenter);

        if (m_originalMainLayout) {
            m_originalMainLayout->addWidget(m_timerArea);
        }
    }

    this->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    this->setMinimumSize(0, 0);
    this->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);

    if (compactWidget) {
        compactWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    }

    if (m_lavaLampWidget) {
        m_lavaLampWidget->setBorderRadius(14);
    }

    switchToCompactMode();
}

void CompactTimerWidget::switchToCompactMode() {
    if (!m_mainWidget || !m_mainLayout || !m_originalMainLayout) {
        initializeCompactMode();
        return;
    }

    isExpanded = false;

    if (m_timerArea) {
        startCompactTransition();
    } else {
        if (m_sidebarWidget) {
            m_sidebarWidget->show();
        }
        if (m_centralWidget) {
            m_centralWidget->show();
        }
        if (!compactWidget) {
            setupCompactUi();
        }

        QWidget* newTimerArea = new QWidget(m_centralWidget);
        newTimerArea->setObjectName("timerArea");
        newTimerArea->setFixedHeight(100);
        newTimerArea->setStyleSheet(
            "QWidget {"
            "   background-color: #ecf0f1;"
            "   border-top: 2px solid #bdc3c7;"
            "}"
        );

        QHBoxLayout* newTimerLayout = new QHBoxLayout(newTimerArea);
        newTimerLayout->setContentsMargins(0, 10, 0, 10);
        newTimerLayout->setAlignment(Qt::AlignCenter);

        setParent(newTimerArea);
        newTimerLayout->addWidget(this, 0, Qt::AlignCenter);
        m_originalMainLayout->addWidget(newTimerArea);
        m_timerArea = newTimerArea;

        setFixedHeight(80);
        setMinimumWidth(200);
        setMaximumWidth(400);

        if (mainLayout) {
            delete mainLayout;
        }
        mainLayout = new QVBoxLayout(this);
        mainLayout->setContentsMargins(5, 5, 5, 5);
        mainLayout->setSpacing(5);

        if (compactWidget) {
            compactWidget->show();
            mainLayout->addWidget(compactWidget);
        }
        if (expandedWidget) {
            expandedWidget->hide();
        }

        emit expandedStateChanged(false);
        updateButtonStates();
        updateDisplay();
        show();

        if (m_lavaLampWidget && compactWidget) {
            m_lavaLampWidget->setGeometry(compactWidget->geometry());
            m_lavaLampWidget->setBorderRadius(20);
            m_lavaLampWidget->lower();
        }
    }
}

void CompactTimerWidget::switchToExpandedMode() {
    if (!m_mainWidget || !m_mainLayout) {
        return;
    }

    isExpanded = true;
    if (!expandedWidget) {
        setupExpandedUi();
    }

    startExpandedTransition();

    if (m_lavaLampWidget) {
        m_lavaLampWidget->setGeometry(rect());
        m_lavaLampWidget->setBorderRadius(10);
        m_lavaLampWidget->lower();
    }
}