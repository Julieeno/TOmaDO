#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QMainWindow>
#include <QPushButton>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QWidget>

#include "database/databasemanager.h"

// Forward declarations
class CalendarWidget;
class CompactTimerWidget;
class DailyDashboardWidget;
class GoalDashboardWidget;
class SettingsDialog;
class StatisticsWidget;
class TotalDashboardWidget;
class WeeklyDashboardWidget;

class MainWindow : public QMainWindow   {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow();

private slots:
    void onSidebarItemClicked(int index);

    void onGoalListItemClicked(int index);

    void onCompletedGoalListItemClicked(int index);

    void onGoalItemClicked(int goalId, bool isCompleted);

    void onAddTodoClicked();

    void onAddGoalClicked();

    void onCompletedGoalsToggled();

    void onSettingsClicked();

    void onSettingsChanged();

    void onTimerExpandedStateChanged(bool isExpanded);

    void onEditTodoRequested(int todoId);

private:
    // UI setup methods
    void setupUi();

    void setupSidebar();

    void setupMainArea();

    void setupTimerArea();

    void setupSettingsButton();

    void connectSignals();

    // Data management methods
    void updateGoalsList();

    void updateCompletedGoalsList();

    void updateGoalsScrollPolicy();

    void refreshGoalsList();

    // Core Layout
    QWidget* m_centralWidget{nullptr};
    QHBoxLayout* m_mainLayout{nullptr};

    // Sidebar
    QWidget* m_sidebarWidget{nullptr};
    QVBoxLayout* m_sidebarLayout{nullptr};
    QListWidget* m_navigationList{nullptr};
    QLabel* m_goalsLabel{nullptr};
    QListWidget* m_goalsList{nullptr};
    QLabel* m_completedGoalsLabel{nullptr};
    QListWidget* m_completedGoalsList{nullptr};
    QPushButton* m_addTodoButton{nullptr};
    QPushButton* m_addGoalButton{nullptr};

    // Main Content Area
    QWidget* m_contentWidget{nullptr};
    QVBoxLayout* m_contentLayout{nullptr};
    QStackedWidget* m_stackedWidget{nullptr};

    // Timer Area
    QWidget* m_timerArea{nullptr};
    QHBoxLayout* m_timerLayout{nullptr};

    // Settings
    QPushButton* m_settingsButton{nullptr};
    SettingsDialog* m_settingsDialog{nullptr};

    // Page Widgets
    DailyDashboardWidget* m_dailyDashboardWidget{nullptr};
    WeeklyDashboardWidget* m_weeklyDashboardWidget{nullptr};
    TotalDashboardWidget* m_totalDashboardWidget{nullptr};
    CalendarWidget* m_calendarWidget{nullptr};
    StatisticsWidget* m_statisticsWidget{nullptr};
    CompactTimerWidget* m_timerWidget{nullptr};
    GoalDashboardWidget* m_goalDashboardWidget{nullptr};

    // State Management
    int m_lastMainTabIndex{0};
    bool m_completedGoalsVisible{false};
    bool m_timerIsExpanded{false};

protected:
    void resizeEvent(QResizeEvent *event) override;

    bool eventFilter(QObject* watched, QEvent* event) override;
};

#endif // MAINWINDOW_H