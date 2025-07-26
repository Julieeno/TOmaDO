#include "dashboard/goaldashboardwidget.h"
#include "../../include/dialogs/goal/goaldialog.h"
#include "timer/compacttimerwidget.h"

#include <QHBoxLayout>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QProgressBar>
#include <QPushButton>
#include <QLabel>
#include <QGroupBox>
#include <QGraphicsDropShadowEffect>

GoalDashboardWidget::GoalDashboardWidget(QWidget* parent)
    : BaseDashboardWidget(parent)
    , m_goalId(0)
    , m_goalName("No Goal Selected")
    , m_timerWidget(nullptr)
    , m_goalInfoWidget(nullptr)
    , m_progressBar(nullptr)
    , m_progressLabel(nullptr)
    , m_completionMessageLabel(nullptr)
    , m_editGoalButton(nullptr)
    , m_completeGoalButton(nullptr)
{
    // Apply goal-specific modern styling
    setStyleSheet(
        "GoalDashboardWidget {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "       stop:0 #FFFFF8, stop:0.3 #FFFEF5, stop:0.7 #FFFDF2, stop:1 #FFFCEF);"
        "   border-radius: 20px;"
        "   border: 2px solid #FFE4B5;"
        "   margin: 5px;"
        "}"
    );

    QTimer::singleShot(0, this, [this]() {
        createGoalSpecificUI();
    });
}

void GoalDashboardWidget::setupUi()
{
    BaseDashboardWidget::setupUi();
    createGoalSpecificUI();

    if (m_pendingTodosListLabel) {
        m_pendingTodosListLabel->setText(tr("Goal To-dos"));
    }

    if (m_completedTodosListLabel) {
        m_completedTodosListLabel->setText(tr("Completed Goal To-dos"));
    }

    if (m_goalId > 0) {
        updateGoalInfo();
        updateGoalProgress();
    }
}

void GoalDashboardWidget::createGoalSpecificUI()
{
    if (m_goalInfoWidget) {
        return;
    }

    createTitleButtons();
    createProgressWidget();
    insertProgressWidget();
    connectButtonSignals();

    if (m_pendingTodosListLabel) {
        m_pendingTodosListLabel->setText(tr("Goal To-dos"));
    }

    if (m_completedTodosListLabel) {
        m_completedTodosListLabel->setText(tr("Completed Goal To-dos"));
    }

    this->updateGeometry();
    this->update();
}

void GoalDashboardWidget::createTitleButtons()
{
    // Create a horizontal layout for the title and buttons with the gradient background
    QWidget* titleContainer = new QWidget(this);
    titleContainer->setFixedHeight(80); // Explicit container height
    titleContainer->setStyleSheet(
        "QWidget {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "       stop:0 #FFF5F5, stop:1 #FFE8E8);"
        "   border-radius: 15px;"
        "}"
    );

    QHBoxLayout* titleLayout = new QHBoxLayout(titleContainer);
    titleLayout->setContentsMargins(12, 0, 12, 0);
    titleLayout->setSpacing(15);

    // Remove the title label from main layout and add it to title container
    if (m_titleLabel && m_mainLayout) {
        m_mainLayout->removeWidget(m_titleLabel);

        // Override the title label styling completely to remove height constraints
        m_titleLabel->setStyleSheet(
            "QLabel {"
            "   background: transparent;"
            "   font-size: 30px;"
            "   font-weight: 700;"
            "   color: #A64545;"
            "   border: none;"
            "   padding: 0px;"
            "   margin-left: 5px;"
            "   min-height: auto;"  // Override the base class min-height
            "   max-height: auto;"  // Override any max-height
            "}"
        );

        titleLayout->addWidget(m_titleLabel, 0, Qt::AlignVCenter);
    }

    titleLayout->addStretch();

    // Add completion message label with modern styling
    m_completionMessageLabel = new QLabel(titleContainer);
    m_completionMessageLabel->setStyleSheet(
        "QLabel {"
        "   font-size: 12px;"
        "   font-weight: 600;"
        "   color: #27ae60;"
        "   padding: 4px 8px;"
        "   margin: 0px;"
        "   text-align: center;"
        "   min-height: auto;"
        "   max-height: auto;"
        "}"
    );
    m_completionMessageLabel->setVisible(false);
    m_completionMessageLabel->setAlignment(Qt::AlignCenter);
    titleLayout->addWidget(m_completionMessageLabel, 0, Qt::AlignVCenter);

    // Create modern complete goal button with explicit size control
    m_completeGoalButton = new QPushButton("Complete", titleContainer);
    m_completeGoalButton->setFixedHeight(30);
    m_completeGoalButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_completeGoalButton->setStyleSheet(
        "QPushButton {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "       stop:0 #27ae60, stop:1 #229954);"
        "   color: white;"
        "   border-radius: 12px;"
        "   padding: 5px;"
        "   font-weight: 600;"
        "   font-size: 14px;"
        "}"
        "QPushButton:hover {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "       stop:0 #229954, stop:1 #1e8449);"
        "}"
        "QPushButton:pressed {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "       stop:0 #1e8449, stop:1 #196f3d);"
        "}"
        "QPushButton:disabled {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "       stop:0 #bdc3c7, stop:1 #cedbdb);"
        "   color: #7f8c8d;"
        "}"
    );
    titleLayout->addWidget(m_completeGoalButton, 0, Qt::AlignVCenter);

    m_editGoalButton = new QPushButton("Edit Goal", titleContainer);
    m_editGoalButton->setFixedHeight(30);
    m_editGoalButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_editGoalButton->setStyleSheet(
        "QPushButton {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "       stop:0 #3498db, stop:1 #2980b9);"
        "   color: white;"
        "   border-radius: 12px;"
        "   padding: 5px;"
        "   font-weight: 600;"
        "   font-size: 14px;"
        "}"
        "QPushButton:hover {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "       stop:0 #2980b9, stop:1 #1f5f8b);"
        "}"
        "QPushButton:pressed {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "       stop:0 #1f5f8b, stop:1 #174c6e);"
        "}"
    );
    titleLayout->addWidget(m_editGoalButton, 0, Qt::AlignVCenter);

    // Add the title container to the main layout at the top
    if (m_mainLayout) {
        m_mainLayout->insertWidget(0, titleContainer);
    }

    // Store reference for cleanup
    m_goalInfoWidget = titleContainer;
}

void GoalDashboardWidget::createProgressWidget()
{
    // Create progress container
    QGroupBox* progressGroup = new QGroupBox("Goal Progress", this);
    progressGroup->setStyleSheet(
        "QGroupBox {"
        "   font-size: 16px;"
        "   font-weight: 600;"
        "   color: #2c3e50;"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "       stop:0 #FFFFFF, stop:1 #FFF8F8);"
        "   border: 2px solid #FFE0E0;"
        "   border-radius: 15px;"
        "   margin-top: 10px;"
        "   padding-top: 20px;"
        "}"
        "QGroupBox::title {"
        "   subcontrol-origin: margin;"
        "   left: 15px;"
        "   padding: 5px 15px;"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "       stop:0 #FF6B7A, stop:1 #FF8A9A);"
        "   color: white;"
        "   border-radius: 8px;"
        "   font-weight: 700;"
        "}"
    );

    QVBoxLayout* progressLayout = new QVBoxLayout(progressGroup);
    progressLayout->setContentsMargins(20, 25, 20, 20);
    progressLayout->setSpacing(15);

    // Create modern progress bar
    m_progressBar = new QProgressBar(progressGroup);
    m_progressBar->setMinimum(0);
    m_progressBar->setMaximum(100);
    m_progressBar->setValue(0);
    m_progressBar->setStyleSheet(
        "QProgressBar {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "       stop:0 #ecf0f1, stop:1 #d5dbdb);"
        "   border: 2px solid #bdc3c7;"
        "   border-radius: 12px;"
        "   text-align: center;"
        "   font-weight: 700;"
        "   font-size: 14px;"
        "   color: #2c3e50;"
        "   min-height: 24px;"
        "}"
        "QProgressBar::chunk {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "       stop:0 #FF6B7A, stop:0.5 #FF8A9A, stop:1 #FFB3B3);"
        "   border-radius: 10px;"
        "   margin: 1px;"
        "}"
    );

    // Add shadow effect to progress bar
    QGraphicsDropShadowEffect* progressEffect = new QGraphicsDropShadowEffect(m_progressBar);
    progressEffect->setBlurRadius(6);
    progressEffect->setXOffset(0);
    progressEffect->setYOffset(1);
    progressEffect->setColor(QColor(255, 107, 122, 60));
    m_progressBar->setGraphicsEffect(progressEffect);

    progressLayout->addWidget(m_progressBar);

    // Create progress label
    m_progressLabel = new QLabel("Progress: 0%", progressGroup);
    m_progressLabel->setAlignment(Qt::AlignCenter);
    m_progressLabel->setStyleSheet(
        "QLabel {"
        "   font-size: 14px;"
        "   font-weight: 600;"
        "   color: #2c3e50;"
        "   background: transparent;"
        "   padding: 5px;"
        "}"
    );
    progressLayout->addWidget(m_progressLabel);

    // Store reference for later insertion
    m_progressWidget = progressGroup;
}

void GoalDashboardWidget::insertProgressWidget()
{
    if (!m_progressWidget || !m_mainLayout) {
        return;
    }

    // Insert progress widget before todo sections
    // Find the position after stats widget but before todo headers
    int insertIndex = -1;
    for (int i = 0; i < m_mainLayout->count(); ++i) {
        QLayoutItem* item = m_mainLayout->itemAt(i);
        if (item && item->widget() == m_statsWidget) {
            insertIndex = i + 1;
            break;
        }
    }

    if (insertIndex >= 0) {
        m_mainLayout->insertWidget(insertIndex, m_progressWidget);
    } else {
        // Fallback: add after stats
        m_mainLayout->addWidget(m_progressWidget);
    }
}

void GoalDashboardWidget::connectButtonSignals()
{
    if (m_editGoalButton) {
        connect(m_editGoalButton, &QPushButton::clicked, this, &GoalDashboardWidget::onEditGoalClicked);
    }

    if (m_completeGoalButton) {
        connect(m_completeGoalButton, &QPushButton::clicked, this, &GoalDashboardWidget::onCompleteGoalClicked);
    }
}

void GoalDashboardWidget::setCurrentGoal(int goalId)
{
    m_goalId = goalId;

    if (m_timerWidget) {
        m_timerWidget->setSelectedGoal(goalId);
    }

    if (m_goalId > 0) {
        updateGoalInfo();
        updateGoalProgress();
        updateTitle();
        refreshFromDatabase();
    }
}

void GoalDashboardWidget::setGoalId(int goalId)
{
    setCurrentGoal(goalId);
}

void GoalDashboardWidget::setTimerWidget(CompactTimerWidget* timer)
{
    m_timerWidget = timer;
}

bool GoalDashboardWidget::canCompleteGoal()
{
    if (m_goalId <= 0) {
        return false;
    }

    DatabaseManager::GoalItem goal = DatabaseManager::instance().getGoal(m_goalId);
    return !goal.isCompleted;
}

void GoalDashboardWidget::showEvent(QShowEvent* event)
{
    BaseDashboardWidget::showEvent(event);

    if (m_goalId > 0) {
        updateGoalInfo();
        updateGoalProgress();
    }
}

void GoalDashboardWidget::doUpdateDateDisplay()
{
    if (m_isBeingDestroyed || !m_dateLabel) {
        return;
    }

    // Hide the date label since we're using the title now
    m_dateLabel->setVisible(false);
}

void GoalDashboardWidget::doCalculateStats()
{
    if (m_isBeingDestroyed || m_goalId <= 0) {
        return;
    }

    // Calculate time spent on this specific goal
    const QDate earliestDate(2000, 1, 1);
    const QDate latestDate(2100, 12, 31);
    m_timeSpentSeconds = DatabaseManager::instance().getGoalTimeSpentInPeriod(
        m_goalId, earliestDate, latestDate);

    // Get timer sessions for this goal
    const QDateTime startDate(earliestDate, QTime(0, 0));
    const QDateTime endDate(latestDate, QTime(23, 59, 59));
    const auto allTimerRecords = DatabaseManager::instance().getTimerRecords(startDate, endDate);

    m_timersCompletedCount = 0;
    for (const auto& record : allTimerRecords) {
        if (record.goalId == m_goalId) {
            m_timersCompletedCount++;
        }
    }

    // Update goal progress
    updateGoalProgress();
}

bool GoalDashboardWidget::doShouldIncludeTodo(const DatabaseManager::TodoItem& todo) const
{
    if (m_isBeingDestroyed || m_goalId <= 0) {
        return false;
    }

    // Include todos that belong to this specific goal
    return todo.goalId == m_goalId;
}

QString GoalDashboardWidget::doGetWidgetTitle() const
{
    if (m_goalId <= 0) {
        return tr("No Goal Selected");
    }

    DatabaseManager::GoalItem goal = DatabaseManager::instance().getGoal(m_goalId);
    if (goal.id == -1) {
        return tr("Goal Not Found");
    }

    return QString("%1").arg(goal.title);
}

void GoalDashboardWidget::updateGoalInfo()
{
    if (m_goalId <= 0) {
        m_goalName = "No Goal Selected";
        return;
    }

    DatabaseManager::GoalItem goal = DatabaseManager::instance().getGoal(m_goalId);
    if (goal.id == -1) {
        m_goalName = "Goal Not Found";
        return;
    }

    m_goalName = goal.title;

    // Update completion message visibility
    if (m_completionMessageLabel) {
        m_completionMessageLabel->setVisible(goal.isCompleted);
    }

    // Update button states
    if (m_completeGoalButton) {
        m_completeGoalButton->setEnabled(!goal.isCompleted);
        if (goal.isCompleted) {
            m_completeGoalButton->setText("Completed");
        } else {
            m_completeGoalButton->setText("Mark as Completed");
        }
    }
}

void GoalDashboardWidget::updateGoalProgress()
{
    if (!m_progressBar || !m_progressLabel || m_goalId <= 0) {
        return;
    }

    DatabaseManager::GoalItem goal = DatabaseManager::instance().getGoal(m_goalId);
    if (goal.id == -1) {
        m_progressBar->setValue(0);
        m_progressLabel->setText("Progress: Unknown");
        if (m_completeGoalButton) {
            m_completeGoalButton->setVisible(false);
        }
        m_completionMessageLabel->setVisible(false);
        return;
    }

    int progressPercentage = 0;
    QString progressText;
    bool showCompleteButton = false;

    if (goal.isCompleted) {
        m_completionMessageLabel->setText(tr("Congratulations! You've completed this goal!"));
        m_completionMessageLabel->setVisible(true);
        m_progressBar->setStyleSheet(
            "QProgressBar {"
            "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
            "       stop:0 #ecf0f1, stop:1 #d5dbdb);"
            "   border: 2px solid #bdc3c7;"
            "   border-radius: 12px;"
            "   text-align: center;"
            "   font-weight: 700;"
            "   font-size: 14px;"
            "   color: #2c3e50;"
            "   min-height: 24px;"
            "}"
            "QProgressBar::chunk {"
            "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
            "       stop:0 #27ae60, stop:0.5 #2ecc71, stop:1 #58d68d);"
            "   border-radius: 10px;"
            "   margin: 1px;"
            "}"
        );
    } else {
        m_progressBar->setStyleSheet(
            "QProgressBar {"
            "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
            "       stop:0 #ecf0f1, stop:1 #d5dbdb);"
            "   border: 2px solid #bdc3c7;"
            "   border-radius: 12px;"
            "   text-align: center;"
            "   font-weight: 700;"
            "   font-size: 14px;"
            "   color: #2c3e50;"
            "   min-height: 24px;"
            "}"
            "QProgressBar::chunk {"
            "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
            "       stop:0 #FF6B7A, stop:0.5 #FF8A9A, stop:1 #FFB3B3);"
            "   border-radius: 10px;"
            "   margin: 1px;"
            "}"
        );
    }

    if (goal.estimatedTime > 0) {
        progressPercentage = qMin(100, (m_timeSpentSeconds * 100) / goal.estimatedTime);

        QString timeSpentStr = formatTime(m_timeSpentSeconds);
        QString estimatedStr = formatTime(goal.estimatedTime);

        progressText = QString("Progress: %1% (%2 of %3)")
                      .arg(progressPercentage)
                      .arg(timeSpentStr)
                      .arg(estimatedStr);

        showCompleteButton = (progressPercentage >= 100) && !goal.isCompleted;

        if (progressPercentage >= 100 && !goal.isCompleted) {
            m_completionMessageLabel->setText(tr("It seems you have completed this goal,\ndo you want to set it as completed?"));
            m_completionMessageLabel->setVisible(true);
        }
    }

    m_progressBar->setValue(progressPercentage);
    m_progressLabel->setText(progressText);

    // Update button visibility and state
    if (m_completeGoalButton) {
        m_completeGoalButton->setVisible(showCompleteButton);
        m_completeGoalButton->setEnabled(!goal.isCompleted);
        m_completeGoalButton->setText(goal.isCompleted ? "Completed" : "Complete");
    }
}

void GoalDashboardWidget::onEditGoalClicked()
{
    if (m_goalId <= 0) {
        return;
    }

    DatabaseManager::GoalItem goal = DatabaseManager::instance().getGoal(m_goalId);
    if (goal.id == -1) {
        QMessageBox::warning(this, tr("Error"), tr("Goal not found."));
        return;
    }

    GoalDialog dialog(goal, this);
    if (dialog.exec() == QDialog::Accepted) {
        DatabaseManager::GoalItem updatedGoal = dialog.getGoal();
        if (DatabaseManager::instance().updateGoal(updatedGoal)) {
            updateGoalInfo();
            updateGoalProgress();
            updateTitle();
            refreshFromDatabase();

            // Notify about the update
            emit databaseUpdated();
        } else {
            QMessageBox::warning(this, tr("Error"), tr("Failed to update goal."));
        }
    }
}

void GoalDashboardWidget::onCompleteGoalClicked()
{
    if (m_goalId <= 0 || !canCompleteGoal()) {
        return;
    }

    int result = QMessageBox::question(this, tr("Complete Goal"),
                                     tr("Are you sure you want to mark this goal as completed?"),
                                     QMessageBox::Yes | QMessageBox::No,
                                     QMessageBox::No);

    if (result == QMessageBox::Yes) {
        if (DatabaseManager::instance().completeGoal(m_goalId)) {
            updateGoalInfo();
            updateGoalProgress();
            refreshFromDatabase();

            // Notify about the completion
            emit databaseUpdated();

            // Show celebration message
            QMessageBox::information(this, tr("Congratulations!"),
                                   tr("Goal completed successfully! Great job! 🎉"));
        } else {
            QMessageBox::warning(this, tr("Error"), tr("Failed to complete goal."));
        }
    }
}

