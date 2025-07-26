#ifndef GOALDASHBOARDWIDGET_H
#define GOALDASHBOARDWIDGET_H

#include <QPushButton>
#include <QProgressBar>
#include <QTimer>
#include <optional>

#include "dashboard/basedashboardwidget.h"
#include "database/databasemanager.h"


class CompactTimerWidget;

class GoalDashboardWidget : public BaseDashboardWidget  {
    Q_OBJECT

public:
    explicit GoalDashboardWidget(QWidget* parent = nullptr);

    // Goal management methods
    void setCurrentGoal(int goalId);

    void setGoalId(int goalId);

    void setTimerWidget(CompactTimerWidget* timer);

    bool canCompleteGoal();

protected:
    void setupUi() override;
    void showEvent(QShowEvent* event) override;

    // BaseDashboardWidget interface implementation
    void doUpdateDateDisplay() override;

    void doCalculateStats() override;

    bool doShouldIncludeTodo(const DatabaseManager::TodoItem& todo) const override;

    QString doGetWidgetTitle() const override;

private slots:
    void onEditGoalClicked();

    void onCompleteGoalClicked();

private:
    // UI creation methods
    void createGoalSpecificUI();

    void createButtonsWidget();

    void createProgressWidget();

    void createTitleButtons();

    void insertProgressWidget();

    void connectButtonSignals();

    // UI helper methods
    QPushButton* createStyledButton(const QString& text, int width, int height,
                                   const QString& bgColor, const QString& hoverColor,
                                   const QString& pressColor);

    // Layout helper methods
    void addButtonsToTitle(QWidget* buttonsWidget);


    int findInsertPosition();

    // Goal data management
    void updateGoalInfo();

    void updateGoalProgress();

    void updateProgressDisplay(const DatabaseManager::GoalItem& goal);

    void setProgressBarStyle(bool isComplete);

    void resetProgress();

    void updateCompletionMessage(bool showMessage);

    // Statistics calculation methods
    void calculateTimeStats();

    void calculateTodoStats();

    void updateTodoLabels();

    // Goal operation methods
    std::optional<DatabaseManager::GoalItem> findGoalById(int goalId);

    int calculateProgressPercentage(int timeSpentSeconds, int estimatedTime);

    bool confirmGoalCompletion();

    void completeGoal();

    // Goal data
    int m_goalId;
    QString m_goalName;
    CompactTimerWidget* m_timerWidget;

    // UI components
    QWidget* m_goalInfoWidget;
    QWidget* m_progressWidget;
    QProgressBar* m_progressBar;
    QLabel* m_progressLabel;
    QLabel* m_completionMessageLabel;
    QPushButton* m_editGoalButton;
    QPushButton* m_completeGoalButton;
};

#endif // GOALDASHBOARDWIDGET_H