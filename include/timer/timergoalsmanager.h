#ifndef TIMERGOALSMANAGER_H
#define TIMERGOALSMANAGER_H

#include <QObject>
#include <QComboBox>
#include <QLabel>
#include <QString>

class TimerGoalsManager : public QObject {
    Q_OBJECT

public:
    explicit TimerGoalsManager(QObject* parent = nullptr);
    ~TimerGoalsManager() override = default;

    void setUIComponents(QComboBox* goalSelectionCombo,
                        QLabel* goalHeaderLabel, 
                        QLabel* todayTimeSpentLabel);

    void populateGoalsComboBox();

    void refreshGoalsList();

    void setSelectedGoal(int goalId);

    void updateGoalHeader(int currentGoalId, const QString& currentTitle);

    void updateTodayTimeSpent();

    int currentSelectedGoalId() const;

signals:
    void goalSelectionChanged(int goalId, const QString& title, const QString& description);

private slots:
    void onGoalSelectionChanged();

private:
    // UI Components
    QComboBox* m_goalSelectionCombo;
    QLabel* m_goalHeaderLabel;
    QLabel* m_todayTimeSpentLabel;
};

#endif // TIMERGOALSMANAGER_H