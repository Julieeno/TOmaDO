#include "../include/mainwindow.h"
#include "database/databasemanager.h"
#include "../include/dialogs/goal/goaldialog.h"
#include "timer/compacttimerwidget.h"
#include "calendar/calendarwidget.h"
#include "statistics/statisticswidget.h"
#include "dashboard/goaldashboardwidget.h"
#include "dashboard/dailydashboardwidget.h"
#include "dashboard/weeklydashboardwidget.h"
#include "dashboard/totaldashboardwidget.h"
#include "settings/settingsdialog.h"
#include "../include/dialogs/todo/tododialog.h"

#include <QVBoxLayout>
#include <QStackedWidget>
#include <QListWidget>
#include <QWidget>
#include <QLabel>
#include <QDebug>
#include <QEvent>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QStyleOptionViewItem>
#include <QApplication>

// Goal item delegate for custom rendering of goals in the list
class GoalItemDelegate : public QStyledItemDelegate {
public:
    explicit GoalItemDelegate(QObject* parent = nullptr) : QStyledItemDelegate(parent) {}

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override {
        QStyledItemDelegate::paint(painter, option, index);

        // Get the goal data
        int goalId = index.data(Qt::UserRole).toInt();
        if (goalId <= 0) return;

        // Find the goal to get its color
        DatabaseManager& db = DatabaseManager::instance();
        auto goals = db.getAllGoals(true); // Get all goals including completed
        QString color = "#FF6B7A"; // tomato red default
        bool isCompleted = false;

        for (const auto& goal : goals) {
            if (goal.id == goalId) {
                color = goal.colorCode.isEmpty() ? "#FF6B7A" : goal.colorCode;
                isCompleted = goal.isCompleted;
                break;
            }
        }

        // Draw colored bullet point
        painter->save();

        // For completed goals, make the color more subdued
        if (isCompleted) {
            QColor completedColor(color);
            completedColor.setAlpha(150); // Make it semi-transparent
            painter->setPen(completedColor);
        } else {
            painter->setPen(QColor(color));
        }

        QFont font = painter->font();
        font.setPointSize(12);
        font.setBold(true);
        painter->setFont(font);

        QRect bulletRect = option.rect;
        bulletRect.setLeft(bulletRect.left() + 12);
        bulletRect.setWidth(16);

        // Use different symbol for completed goals if they don't already have checkmark
        QString bullet = isCompleted && !index.data(Qt::DisplayRole).toString().contains("✓") ? "✓" : "●";
        painter->drawText(bulletRect, Qt::AlignLeft | Qt::AlignVCenter, bullet);
        painter->restore();
    }
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_centralWidget(nullptr)
    , m_mainLayout(nullptr)
    , m_sidebarWidget(nullptr)
    , m_sidebarLayout(nullptr)
    , m_navigationList(nullptr)
    , m_goalsList(nullptr)
    , m_addTodoButton(nullptr)
    , m_addGoalButton(nullptr)
    , m_lastMainTabIndex(0)
    , m_completedGoalsVisible(false)
    , m_contentWidget(nullptr)
    , m_contentLayout(nullptr)
    , m_stackedWidget(nullptr)
    , m_timerArea(nullptr)
    , m_timerLayout(nullptr)
    , m_timerIsExpanded(false)
    , m_settingsButton(nullptr)
    , m_settingsDialog(nullptr)
    , m_dailyDashboardWidget(nullptr)
    , m_weeklyDashboardWidget(nullptr)
    , m_totalDashboardWidget(nullptr)
    , m_calendarWidget(nullptr)
    , m_statisticsWidget(nullptr)
    , m_timerWidget(nullptr)
    , m_goalDashboardWidget(nullptr)
{
    setupUi();
}

MainWindow::~MainWindow()
{
    // First, disconnect all signals to prevent calls during destruction
    disconnect(m_navigationList, nullptr, this, nullptr);
    disconnect(m_goalsList, nullptr, this, nullptr);
    disconnect(m_addTodoButton, nullptr, this, nullptr);
    disconnect(m_addGoalButton, nullptr, this, nullptr);
    disconnect(m_settingsButton, nullptr, this, nullptr);

    if (m_dailyDashboardWidget) {
        disconnect(m_dailyDashboardWidget, nullptr, this, nullptr);
    }

    if (m_weeklyDashboardWidget) {
        disconnect(m_weeklyDashboardWidget, nullptr, this, nullptr);
    }

    if (m_totalDashboardWidget) {
        disconnect(m_totalDashboardWidget, nullptr, this, nullptr);
    }

    if (m_goalDashboardWidget) {
        disconnect(m_goalDashboardWidget, nullptr, this, nullptr);
    }

    if (m_timerWidget) {
        disconnect(m_timerWidget, nullptr, this, nullptr);
    }

    // Delete widgets in a controlled order to prevent issues
    delete m_settingsDialog;
    m_settingsDialog = nullptr;

    // Clear the stacked widget's contents first
    if (m_stackedWidget) {
        while (m_stackedWidget->count() > 0) {
            QWidget* widget = m_stackedWidget->widget(0);
            m_stackedWidget->removeWidget(widget);
        }
    }

    // Now it's safe to delete the dashboard widgets
    delete m_goalDashboardWidget;
    m_goalDashboardWidget = nullptr;

    delete m_dailyDashboardWidget;
    m_dailyDashboardWidget = nullptr;

    delete m_weeklyDashboardWidget;
    m_weeklyDashboardWidget = nullptr;

    delete m_totalDashboardWidget;
    m_totalDashboardWidget = nullptr;

    delete m_calendarWidget;
    m_calendarWidget = nullptr;

    delete m_statisticsWidget;
    m_statisticsWidget = nullptr;

    delete m_timerWidget;
    m_timerWidget = nullptr;

    // Finally delete containers
    delete m_stackedWidget;
    m_stackedWidget = nullptr;
}

void MainWindow::setupUi()
{
    // Set main window styling
    setStyleSheet(
        "QMainWindow {"
        "   background-color: #FFF8F5;"
        "   color: #2C3E50;"
        "   font-family: 'Segoe UI', 'Roboto', 'Arial', sans-serif;"
        "}"
    );

    // Create main container widget
    m_centralWidget = new QWidget(this);
    m_centralWidget->setStyleSheet(
        "QWidget {"
        "   background-color: #FFF8F5;"
        "   border: none;"
        "}"
    );

    m_mainLayout = new QHBoxLayout(m_centralWidget);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);

    setupSidebar();
    setupMainArea();
    setupTimerArea();
    setupSettingsButton();
    connectSignals();

    setCentralWidget(m_centralWidget);
    setWindowTitle("TOmaDO - Goals & Timer Manager");
    resize(1300, 850);
    setMinimumSize(1100, 750);

    // Set initial page to dashboard
    m_navigationList->setCurrentRow(0);
    m_stackedWidget->setCurrentIndex(0);

    // Load goals
    refreshGoalsList();

    if (m_goalDashboardWidget && m_timerWidget) {
        m_goalDashboardWidget->setTimerWidget(m_timerWidget);
    }
}

void MainWindow::setupSidebar()
{
    m_sidebarWidget = new QWidget(m_centralWidget);
    m_sidebarWidget->setFixedWidth(320);
    m_sidebarWidget->setStyleSheet(
        "QWidget {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "               stop:0 #FFE8EA, stop:1 #FFF0F1);"
        "   border: none;"
        "   border-top-right-radius: 16px;"
        "   border-bottom-right-radius: 16px;"
        "}"
    );

    m_sidebarLayout = new QVBoxLayout(m_sidebarWidget);
    m_sidebarLayout->setContentsMargins(0, 15, 0, 15);
    m_sidebarLayout->setSpacing(5);

    // App title with tomato emoji
    QLabel* titleLabel = new QLabel("TOmaDO", m_sidebarWidget);
    titleLabel->setStyleSheet(
        "QLabel {"
        "   color: #E85A69;"
        "   font-size: 24px;"
        "   font-weight: bold;"
        "   padding: 15px;"
        "   margin-bottom: 5px;"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "               stop:0 rgba(255,107,122,0.1), stop:1 rgba(255,107,122,0.05));"
        "   border-radius: 12px;"
        "   margin-left: 10px;"
        "   margin-right: 10px;"
        "}"
    );
    titleLabel->setAlignment(Qt::AlignCenter);
    m_sidebarLayout->addWidget(titleLabel);

    // Navigation list
    m_navigationList = new QListWidget(m_sidebarWidget);
    m_navigationList->setSpacing(12);
    m_navigationList->setStyleSheet(
        "QListWidget {"
        "   background-color: transparent;"
        "   border: none;"
        "   outline: none;"
        "   padding: 0 15px;"
        "}"
        "QListWidget::item {"
        "   color: #2C3E50;"
        "   font-size: 14px;"
        "   font-weight: 500;"
        "   padding: 10px 16px;"
        "   margin: 2px 0;"
        "   border-radius: 10px;"
        "   background-color: rgba(255,255,255,0.6);"
        "   border: 2px solid transparent;"
        "}"
        "QListWidget::item:selected {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "               stop:0 #FF6B7A, stop:1 #E85A69);"
        "   color: white;"
        "   font-weight: bold;"
        "   border: 2px solid #E85A69;"
        "}"
        "QListWidget::item:hover:!selected {"
        "   background-color: rgba(255,107,122,0.1);"
        "   color: #E85A69;"
        "   border: 2px solid rgba(255,107,122,0.3);"
        "}"
    );

    m_navigationList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_navigationList->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QStringList navigationItems = {
        "📊 Daily Dashboard",
        "≣ Weekly Dashboard",
        "📉 Total Dashboard",
        "◍ Calendar",
        "📈 Statistics"
    };
    for (const QString& item : navigationItems) {
        m_navigationList->addItem(item);
    }

    int itemHeight = 50;
    int navigationHeight = navigationItems.size() * itemHeight + 20;
    m_navigationList->setFixedHeight(navigationHeight);
    m_navigationList->setVerticalScrollMode(QListView::ScrollPerPixel);
    m_navigationList->setProperty("scrollable", false);
    m_navigationList->viewport()->installEventFilter(this);

    m_sidebarLayout->addWidget(m_navigationList);

    // Goals section header
    m_goalsLabel = new QLabel("GOALS", m_sidebarWidget);
    m_goalsLabel->setStyleSheet(
        "QLabel {"
        "   color: #E85A69;"
        "   font-size: 12px;"
        "   font-weight: bold;"
        "   padding: 8px 25px 4px 25px;"
        "   letter-spacing: 1px;"
        "}"
    );
    m_sidebarLayout->addWidget(m_goalsLabel);

    // Pending goals list - NO stretch factor, will size to content
    m_goalsList = new QListWidget(m_sidebarWidget);
    m_goalsList->setStyleSheet(
        "QListWidget {"
        "   background-color: transparent;"
        "   border: none;"
        "   outline: none;"
        "   padding: 0 15px;"
        "}"
        "QListWidget::item {"
        "   color: #2C3E50;"
        "   font-size: 11px;"
        "   font-weight: 500;"
        "   padding: 8px 12px 8px 20px;"
        "   margin: 2px 0;"
        "   border-radius: 8px;"
        "   border-left: 3px solid transparent;"
        "}"
        "QListWidget::item:hover {"
        "   background-color: rgba(255,107,122,0.1);"
        "   color: #E85A69;"
        "   border-left: 3px solid #FF6B7A;"
        "}"
        "QListWidget::item:selected {"
        "   background-color: rgba(255,107,122,0.15);"
        "   color: #E85A69;"
        "   border-left: 3px solid #FF6B7A;"
        "   font-weight: bold;"
        "}"
        "QScrollBar:vertical {"
        "   background-color: rgba(255,255,255,0.3);"
        "   width: 6px;"
        "   border-radius: 3px;"
        "   margin: 0;"
        "}"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {"
        "   height: 0px;"
        "   width: 0px;"
        "   subcontrol-origin: margin;"
        "   subcontrol-position: bottom;"
        "}"
        "QScrollBar::handle:vertical {"
        "   background-color: rgba(255,107,122,0.5);"
        "   border-radius: 3px;"
        "   min-height: 20px;"
        "}"
        "QScrollBar::handle:vertical:hover {"
        "   background-color: rgba(255,107,122,0.7);"
        "}"
    );

    m_goalsList->setItemDelegate(new GoalItemDelegate(this));
    // Set size policy to allow vertical growth but prefer minimum size
    m_goalsList->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    m_sidebarLayout->addWidget(m_goalsList); // No stretch factor

    // Completed goals section
    m_completedGoalsLabel = new QLabel("COMPLETED GOALS ▶", m_sidebarWidget);
    m_completedGoalsLabel->setStyleSheet(
        "QLabel {"
        "   color: #E85A69;"
        "   font-size: 12px;"
        "   font-weight: bold;"
        "   padding: 2px 25px 4px 25px;"
        "   letter-spacing: 1px;"
        "}"
        "QLabel:hover {"
        "   color: #D44A5A;"
        "}"
    );
    m_completedGoalsLabel->installEventFilter(this);
    m_sidebarLayout->addWidget(m_completedGoalsLabel);

    // Completed goals list
    m_completedGoalsList = new QListWidget(m_sidebarWidget);
    m_completedGoalsList->setStyleSheet(
        "QListWidget {"
        "   background-color: transparent;"
        "   border: none;"
        "   outline: none;"
        "   padding: 0 15px;"
        "}"
        "QListWidget::item {"
        "   color: #2C3E50;"
        "   font-size: 11px;"
        "   font-weight: 500;"
        "   padding: 8px 12px 8px 20px;"
        "   margin: 2px 0;"
        "   border-radius: 8px;"
        "   border-left: 3px solid transparent;"
        "}"
        "QListWidget::item:hover {"
        "   background-color: rgba(255,107,122,0.1);"
        "   color: #E85A69;"
        "   border-left: 3px solid #FF6B7A;"
        "}"
        "QListWidget::item:selected {"
        "   background-color: rgba(255,107,122,0.15);"
        "   color: #E85A69;"
        "   border-left: 3px solid #FF6B7A;"
        "   font-weight: bold;"
        "}"
        "QScrollBar:vertical {"
        "   background-color: rgba(255,255,255,0.3);"
        "   width: 6px;"
        "   border-radius: 3px;"
        "   margin: 0;"
        "}"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {"
        "   height: 0px;"
        "   width: 0px;"
        "   subcontrol-origin: margin;"
        "   subcontrol-position: bottom;"
        "}"
        "QScrollBar::handle:vertical {"
        "   background-color: rgba(255,107,122,0.5);"
        "   border-radius: 3px;"
        "   min-height: 20px;"
        "}"
        "QScrollBar::handle:vertical:hover {"
        "   background-color: rgba(255,107,122,0.7);"
        "}"
    );
    m_completedGoalsList->setItemDelegate(new GoalItemDelegate(this));
    m_completedGoalsList->hide();
    m_completedGoalsList->setMaximumHeight(120);
    m_sidebarLayout->addWidget(m_completedGoalsList);

    // Add stretch to push buttons to bottom
    m_sidebarLayout->addStretch();

    // Buttons at the bottom
    QString buttonStyle =
        "QPushButton {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "               stop:0 #FF6B7A, stop:1 #E85A69);"
        "   color: white;"
        "   font-size: 12px;"
        "   font-weight: bold;"
        "   padding: 10px 16px;"
        "   border: none;"
        "   border-radius: 10px;"
        "   margin: 3px 15px;"
        "}"
        "QPushButton:hover {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "               stop:0 #E85A69, stop:1 #D44A5A);"
        "}"
        "QPushButton:pressed {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "               stop:0 #D44A5A, stop:1 #C23A4A);"
        "}";

    m_addTodoButton = new QPushButton("+ Add Todo", m_sidebarWidget);
    m_addTodoButton->setStyleSheet(buttonStyle);
    m_sidebarLayout->addWidget(m_addTodoButton);

    m_addGoalButton = new QPushButton("+ Add Goal", m_sidebarWidget);
    m_addGoalButton->setStyleSheet(buttonStyle);
    m_sidebarLayout->addWidget(m_addGoalButton);

    m_mainLayout->addWidget(m_sidebarWidget);
}

void MainWindow::setupMainArea()
{
    m_contentWidget = new QWidget(m_centralWidget);
    m_contentWidget->setStyleSheet(
        "QWidget {"
        "   background-color: #FFF8F5;"
        "   border: none;"
        "}"
    );

    m_contentLayout = new QVBoxLayout(m_contentWidget);
    m_contentLayout->setContentsMargins(25, 25, 25, 15);
    m_contentLayout->setSpacing(20);

    // Create stacked widget for different views
    m_stackedWidget = new QStackedWidget(m_contentWidget);
    m_stackedWidget->setStyleSheet(
        "QStackedWidget {"
        "   background-color: white;"
        "   border-radius: 16px;"
        "   border: none;"
        "}"
    );

    // Create dashboard widgets
    m_dailyDashboardWidget = new DailyDashboardWidget(m_stackedWidget);
    m_weeklyDashboardWidget = new WeeklyDashboardWidget(m_stackedWidget);
    m_totalDashboardWidget = new TotalDashboardWidget(m_stackedWidget);
    m_calendarWidget = new CalendarWidget(m_stackedWidget);
    m_statisticsWidget = new StatisticsWidget(m_stackedWidget);

    // Add widgets to stacked widget
    m_stackedWidget->addWidget(m_dailyDashboardWidget);
    m_stackedWidget->addWidget(m_weeklyDashboardWidget);
    m_stackedWidget->addWidget(m_totalDashboardWidget);
    m_stackedWidget->addWidget(m_calendarWidget);
    m_stackedWidget->addWidget(m_statisticsWidget);

    m_contentLayout->addWidget(m_stackedWidget);
    m_mainLayout->addWidget(m_contentWidget);

    // Create goal dashboard - ADD TO STACKED WIDGET
    m_goalDashboardWidget = new GoalDashboardWidget(m_stackedWidget);
    m_stackedWidget->addWidget(m_goalDashboardWidget);
}

void MainWindow::setupTimerArea()
{
    m_timerArea = new QWidget(m_contentWidget);
    m_timerArea->setObjectName("timerArea");
    m_timerArea->setFixedHeight(100);
    m_timerArea->setStyleSheet(
        "QWidget#timerArea {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "               stop:0 rgba(255,107,122,0.05), stop:1 rgba(232,90,105,0.05));"
        "   border-top: 2px solid rgba(255,107,122,0.3);"
        "   border-radius: 12px;"
        "   margin-top: 10px;"
        "}"
    );

    m_timerLayout = new QHBoxLayout(m_timerArea);
    m_timerLayout->setContentsMargins(0, 15, 0, 15);
    m_timerLayout->setAlignment(Qt::AlignCenter);

    // Create timer widget
    m_timerWidget = new CompactTimerWidget(m_timerArea);
    m_timerWidget->setMainWindowComponents(m_stackedWidget, m_contentWidget, this, m_mainLayout);
    m_timerLayout->addWidget(m_timerWidget, 0, Qt::AlignCenter);

    m_contentLayout->addWidget(m_timerArea);
}

void MainWindow::setupSettingsButton()
{
    // Settings button positioned in bottom-right corner
    m_settingsButton = new QPushButton("⚙️", this);
    m_settingsButton->setFixedSize(50, 50);
    m_settingsButton->setStyleSheet(
        "QPushButton {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "               stop:0 rgba(255,107,122,0.9), stop:1 rgba(232,90,105,0.9));"
        "   color: white;"
        "   font-size: 20px;"
        "   border: none;"
        "   border-radius: 25px;"
        "   margin: 0px;"
        "}"
        "QPushButton:hover {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "               stop:0 rgba(232,90,105,1.0), stop:1 rgba(212,74,90,1.0));"
        "   font-size: 22px;"
        "}"
        "QPushButton:pressed {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "               stop:0 rgba(212,74,90,1.0), stop:1 rgba(194,58,74,1.0));"
        "   font-size: 18px;"
        "}"
    );

    // Position in bottom-right corner
    m_settingsButton->move(width() - 70, height() - 70);
    m_settingsButton->raise(); // Ensure it's on top
}

void MainWindow::connectSignals()
{
    connect(m_navigationList, QOverload<int>::of(&QListWidget::currentRowChanged),
            this, &MainWindow::onSidebarItemClicked);

    // Connect goal lists to click handler
    connect(m_goalsList, QOverload<int>::of(&QListWidget::currentRowChanged),
            this, &MainWindow::onGoalListItemClicked);
    connect(m_completedGoalsList, QOverload<int>::of(&QListWidget::currentRowChanged),
            this, &MainWindow::onCompletedGoalListItemClicked);

    connect(m_addTodoButton, &QPushButton::clicked, this, &MainWindow::onAddTodoClicked);
    connect(m_addGoalButton, &QPushButton::clicked, this, &MainWindow::onAddGoalClicked);
    connect(m_settingsButton, &QPushButton::clicked, this, &MainWindow::onSettingsClicked);

    if (m_timerWidget) {
        connect(m_timerWidget, &CompactTimerWidget::expandedStateChanged,
                this, &MainWindow::onTimerExpandedStateChanged);
    }

    if (m_dailyDashboardWidget) {
        connect(m_dailyDashboardWidget, &DailyDashboardWidget::todoEditRequested,
                this, &MainWindow::onEditTodoRequested);
    }

    if (m_weeklyDashboardWidget) {
        connect(m_weeklyDashboardWidget, &WeeklyDashboardWidget::todoEditRequested,
                this, &MainWindow::onEditTodoRequested);
    }

    if (m_totalDashboardWidget) {
        connect(m_totalDashboardWidget, &TotalDashboardWidget::todoEditRequested,
                this, &MainWindow::onEditTodoRequested);
    }

    if (m_goalDashboardWidget) {
        connect(m_goalDashboardWidget, &GoalDashboardWidget::todoEditRequested,
                this, &MainWindow::onEditTodoRequested);
    }

    if (m_settingsDialog) {
        connect(m_settingsDialog, &SettingsDialog::settingsChanged,
                this, &MainWindow::onSettingsChanged);
    }
}

void MainWindow::onSidebarItemClicked(int index)
{
    if (index >= 0 && index < m_stackedWidget->count() - 1) { // -1 because goal dashboard is last
        m_stackedWidget->setCurrentIndex(index);
        m_lastMainTabIndex = index;

        // Clear goal selections when switching to main tabs
        m_goalsList->clearSelection();
        m_completedGoalsList->clearSelection();

        // Refresh the current widget using the correct method name
        QWidget* currentWidget = m_stackedWidget->currentWidget();

        if (currentWidget == m_calendarWidget) {
            m_calendarWidget->refreshFromDatabase();
        } else if (currentWidget == m_statisticsWidget) {
            m_statisticsWidget->refreshFromDatabase();
        } else if (currentWidget == m_dailyDashboardWidget) {
            m_dailyDashboardWidget->refreshFromDatabase();
        } else if (currentWidget == m_weeklyDashboardWidget) {
            m_weeklyDashboardWidget->refreshFromDatabase();
        } else if (currentWidget == m_totalDashboardWidget) {
            m_totalDashboardWidget->refreshFromDatabase();
        }
    }
}

void MainWindow::onGoalListItemClicked(int index)
{
    if (index >= 0) {
        QListWidgetItem* item = m_goalsList->item(index);
        if (item) {
            int goalId = item->data(Qt::UserRole).toInt();

            // Switch to goal dashboard and set the selected goal
            m_stackedWidget->setCurrentIndex(m_stackedWidget->count() - 1); // Goal dashboard is last
            m_navigationList->clearSelection(); // Clear navigation selection
            m_completedGoalsList->clearSelection(); // Clear completed goals selection

            if (m_goalDashboardWidget) {
                m_goalDashboardWidget->setCurrentGoal(goalId);
                m_goalDashboardWidget->refreshFromDatabase();
            }
        }
    }
}

void MainWindow::onCompletedGoalListItemClicked(int index)
{
    if (index >= 0) {
        QListWidgetItem* item = m_completedGoalsList->item(index);
        if (item) {
            int goalId = item->data(Qt::UserRole).toInt();

            // Switch to goal dashboard and set the selected goal
            m_stackedWidget->setCurrentIndex(m_stackedWidget->count() - 1); // Goal dashboard is last
            m_navigationList->clearSelection(); // Clear navigation selection
            m_goalsList->clearSelection(); // Clear pending goals selection

            if (m_goalDashboardWidget) {
                m_goalDashboardWidget->setCurrentGoal(goalId);
                m_goalDashboardWidget->refreshFromDatabase();
            }
        }
    }
}

void MainWindow::onGoalItemClicked(int goalId, bool isCompleted)
{
    Q_UNUSED(goalId)
    Q_UNUSED(isCompleted)
    // Handle goal item clicks if needed - now handled by list item clicked
}

void MainWindow::onCompletedGoalsToggled()
{
    m_completedGoalsVisible = !m_completedGoalsVisible;

    if (m_completedGoalsVisible) {
        m_completedGoalsLabel->setText("COMPLETED GOALS ▼");
        m_completedGoalsList->show();
        updateCompletedGoalsList();

        // Wait for layout to settle, then update
        QTimer::singleShot(50, this, [this]() {
            updateGoalsScrollPolicy();
        });
    } else {
        m_completedGoalsLabel->setText("COMPLETED GOALS ▶");
        m_completedGoalsList->hide();

        // Update immediately when hiding
        QTimer::singleShot(10, this, [this]() {
            updateGoalsScrollPolicy();
        });
    }
}

void MainWindow::onAddTodoClicked()
{
    DatabaseManager::TodoItem newTodo;
    newTodo.id = -1;
    newTodo.title = "";
    newTodo.description = "";
    newTodo.priority = DatabaseManager::TodoPriority::Medium;
    newTodo.isCompleted = false;
    newTodo.goalId = -1;
    newTodo.colorCode = "#FF6B7A"; // Default tomato color

    TodoDialog dialog(newTodo, this);
    if (dialog.exec() == QDialog::Accepted) {
        DatabaseManager::TodoItem todoToAdd = dialog.getTodo();
        if (DatabaseManager::instance().addTodo(todoToAdd)) {
            refreshGoalsList();

            // Refresh current view
            QWidget* currentWidget = m_stackedWidget->currentWidget();
            if (currentWidget == m_dailyDashboardWidget) {
                m_dailyDashboardWidget->refreshFromDatabase();
            } else if (currentWidget == m_weeklyDashboardWidget) {
                m_weeklyDashboardWidget->refreshFromDatabase();
            } else if (currentWidget == m_totalDashboardWidget) {
                m_totalDashboardWidget->refreshFromDatabase();
            } else if (currentWidget == m_goalDashboardWidget) {
                m_goalDashboardWidget->refreshFromDatabase();
            }
        }
    }
}

void MainWindow::onAddGoalClicked()
{
    DatabaseManager::GoalItem newGoal;
    newGoal.id = -1;
    newGoal.title = "";
    newGoal.description = "";
    newGoal.priority = DatabaseManager::TodoPriority::Medium;
    newGoal.colorCode = "#FF6B7A"; // Default tomato color
    newGoal.estimatedTime = 0;
    newGoal.isCompleted = false;

    GoalDialog dialog(newGoal, this);
    if (dialog.exec() == QDialog::Accepted) {
        DatabaseManager::GoalItem goalToAdd = dialog.getGoal();
        if (DatabaseManager::instance().addGoal(goalToAdd)) {
            refreshGoalsList();

            // Refresh current view
            QWidget* currentWidget = m_stackedWidget->currentWidget();
            if (currentWidget == m_dailyDashboardWidget) {
                m_dailyDashboardWidget->refreshFromDatabase();
            } else if (currentWidget == m_weeklyDashboardWidget) {
                m_weeklyDashboardWidget->refreshFromDatabase();
            } else if (currentWidget == m_totalDashboardWidget) {
                m_totalDashboardWidget->refreshFromDatabase();
            } else if (currentWidget == m_goalDashboardWidget) {
                m_goalDashboardWidget->refreshFromDatabase();
            }
        }
    }
}

void MainWindow::onSettingsClicked()
{
    if (!m_settingsDialog) {
        m_settingsDialog = new SettingsDialog(this);
        connect(m_settingsDialog, &SettingsDialog::settingsChanged,
                this, &MainWindow::onSettingsChanged);
    }
    m_settingsDialog->exec();
}

void MainWindow::onSettingsChanged()
{
    if (m_timerWidget) {
        m_timerWidget->refreshSettings();
    }

}

void MainWindow::onTimerExpandedStateChanged(bool isExpanded)
{
    m_timerIsExpanded = isExpanded;

    if (isExpanded) {
        m_settingsButton->hide();
        m_timerArea->hide();
    } else {
        m_settingsButton->show();
        m_timerArea->show();
    }
}

void MainWindow::onEditTodoRequested(int todoId)
{
    // Get the todo item from database using singleton instance
    auto todos = DatabaseManager::instance().getAllTodos(); // Get all todos
    DatabaseManager::TodoItem todoToEdit;
    bool found = false;

    for (const auto& todo : todos) {
        if (todo.id == todoId) {
            todoToEdit = todo;
            found = true;
            break;
        }
    }

    if (!found) {
        QMessageBox::warning(this, tr("Error"), tr("Todo not found."));
        return;
    }

    // Create and show the TodoDialog for editing
    TodoDialog dialog(todoToEdit, this);
    if (dialog.exec() == QDialog::Accepted) {
        DatabaseManager::TodoItem updatedTodo = dialog.getTodo();

        // Update the todo in database
        if (DatabaseManager::instance().updateTodo(updatedTodo)) {
            // Refresh all dashboard widgets (correct method name)
            if (m_dailyDashboardWidget) {
                m_dailyDashboardWidget->refreshFromDatabase();
            }
            if (m_weeklyDashboardWidget) {
                m_weeklyDashboardWidget->refreshFromDatabase();
            }
            if (m_totalDashboardWidget) {
                m_totalDashboardWidget->refreshFromDatabase();
            }
            if (m_goalDashboardWidget) {
                m_goalDashboardWidget->refreshFromDatabase();
            }
        } else {
            QMessageBox::warning(this, tr("Error"), tr("Failed to update todo."));
        }
    }
}

void MainWindow::updateGoalsList()
{
    // Update pending goals list
    m_goalsList->clear();
    auto allGoals = DatabaseManager::instance().getAllGoals(true);

    for (const auto& goal : allGoals) {
        if (!goal.isCompleted) {
            QListWidgetItem* item = new QListWidgetItem(goal.title);
            item->setData(Qt::UserRole, goal.id);
            m_goalsList->addItem(item);
        }
    }

    // Update completed goals list if visible
    if (m_completedGoalsVisible) {
        updateCompletedGoalsList();
    }

    updateGoalsScrollPolicy();
}

void MainWindow::updateCompletedGoalsList()
{
    m_completedGoalsList->clear();
    auto allGoals = DatabaseManager::instance().getAllGoals(true);

    for (const auto& goal : allGoals) {
        if (goal.isCompleted) {
            QString displayText = "✓ " + goal.title;
            QListWidgetItem* item = new QListWidgetItem(displayText);
            item->setData(Qt::UserRole, goal.id);
            m_completedGoalsList->addItem(item);
        }
    }
}

void MainWindow::updateGoalsScrollPolicy()
{
    // Force layout updates to ensure accurate measurements
    m_sidebarWidget->updateGeometry();
    QApplication::processEvents();

    // Calculate available space for pending goals
    int sidebarHeight = m_sidebarWidget->height();
    int usedSpace = 0;

    // Calculate space used by other elements with actual measurements
    usedSpace += m_sidebarLayout->contentsMargins().top() + m_sidebarLayout->contentsMargins().bottom();

    // Get actual heights of existing widgets
    for (int i = 0; i < m_sidebarLayout->count(); ++i) {
        QLayoutItem* item = m_sidebarLayout->itemAt(i);
        if (!item) continue;

        QWidget* widget = item->widget();
        if (widget && widget != m_goalsList) {
            if (widget == m_completedGoalsList) {
                // Only count completed goals if visible
                if (m_completedGoalsVisible && widget->isVisible()) {
                    usedSpace += widget->height();
                }
            } else {
                usedSpace += widget->height();
            }
        } else if (item->spacerItem()) {
            // Don't count stretch spacers in used space
            continue;
        }
    }

    // Add spacing between items
    int visibleItems = 0;
    for (int i = 0; i < m_sidebarLayout->count(); ++i) {
        QLayoutItem* item = m_sidebarLayout->itemAt(i);
        if (item->widget() && item->widget()->isVisible()) {
            visibleItems++;
        }
    }
    usedSpace += (visibleItems - 1) * m_sidebarLayout->spacing();

    // Calculate available space for pending goals
    int availableSpace = sidebarHeight - usedSpace;
    availableSpace = qMax(availableSpace, 60); // Ensure minimum space

    // Calculate height based on content
    int itemCount = m_goalsList->count();
    int itemHeight = 32;
    int minHeight = 60;

    int calculatedHeight;
    if (itemCount == 0) {
        calculatedHeight = minHeight;
    } else {
        int contentHeight = itemCount * itemHeight + 20;
        calculatedHeight = qMin(contentHeight, availableSpace);
        calculatedHeight = qMax(calculatedHeight, minHeight);
    }

    m_goalsList->setFixedHeight(calculatedHeight);

    // Set scroll policy based on whether content fits
    int totalContentHeight = itemCount * itemHeight + 20;
    if (totalContentHeight > calculatedHeight) {
        m_goalsList->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    } else {
        m_goalsList->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }
}

void MainWindow::refreshGoalsList()
{
    updateGoalsList();
    // Update after a brief delay to ensure list is populated
    QTimer::singleShot(25, this, &MainWindow::updateGoalsScrollPolicy);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);

    // Reposition settings button in bottom-right corner
    if (m_settingsButton) {
        m_settingsButton->move(width() - 70, height() - 70);
    }

    // Recalculate after resize
    QTimer::singleShot(25, this, &MainWindow::updateGoalsScrollPolicy);
}

bool MainWindow::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == m_navigationList->viewport() && event->type() == QEvent::Wheel) {
        // Block wheel events on navigation list to prevent scrolling
        return true;
    }

    // Handle completed goals label clicks
    if (watched == m_completedGoalsLabel && event->type() == QEvent::MouseButtonPress) {
        onCompletedGoalsToggled();
        return true;
    }

    return QMainWindow::eventFilter(watched, event);
}