#include "timer/timergoalsmanager.h"
#include "database/databasemanager.h"
#include <QComboBox>
#include <QLabel>
#include <QDateTime>
#include <QDebug>

TimerGoalsManager::TimerGoalsManager(QObject* parent)
    : QObject(parent)
    , m_goalSelectionCombo(nullptr)
    , m_goalHeaderLabel(nullptr)
    , m_todayTimeSpentLabel(nullptr)
{
}

void TimerGoalsManager::setUIComponents(QComboBox* goalSelectionCombo,
                                       QLabel* goalHeaderLabel,
                                       QLabel* todayTimeSpentLabel) {
    if (m_goalSelectionCombo) {
        disconnect(m_goalSelectionCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
                   this, &TimerGoalsManager::onGoalSelectionChanged);
    }

    m_goalSelectionCombo = goalSelectionCombo;
    m_goalHeaderLabel = goalHeaderLabel;
    m_todayTimeSpentLabel = todayTimeSpentLabel;

    // Connect new signals if combo box is valid
    if (m_goalSelectionCombo) {
        connect(m_goalSelectionCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, &TimerGoalsManager::onGoalSelectionChanged);
    }
}

void TimerGoalsManager::populateGoalsComboBox() {
    if (!m_goalSelectionCombo) {
        return;
    }

    // Store current selection
    int currentGoalId = currentSelectedGoalId();

    // Temporarily disconnect signal to avoid triggering changes during refresh
    disconnect(m_goalSelectionCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
               this, &TimerGoalsManager::onGoalSelectionChanged);

    m_goalSelectionCombo->clear();
    m_goalSelectionCombo->addItem("Select Goal", -1);

    DatabaseManager& db = DatabaseManager::instance();
    QList<DatabaseManager::GoalItem> goals = db.getAllGoals();

    for (const auto& goal : goals) {
        m_goalSelectionCombo->addItem(goal.title, goal.id);
    }

    // Restore selection if it was valid
    if (currentGoalId != -1) {
        setSelectedGoal(currentGoalId);
    }

    // Reconnect signal
    connect(m_goalSelectionCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &TimerGoalsManager::onGoalSelectionChanged);
}

void TimerGoalsManager::refreshGoalsList() {
    populateGoalsComboBox();
    updateTodayTimeSpent();
}

void TimerGoalsManager::setSelectedGoal(int goalId) {
    if (!m_goalSelectionCombo) {
        return;
    }

    for (int i = 0; i < m_goalSelectionCombo->count(); ++i) {
        if (m_goalSelectionCombo->itemData(i).toInt() == goalId) {
            m_goalSelectionCombo->setCurrentIndex(i);
            break;
        }
    }
}

void TimerGoalsManager::updateGoalHeader(int currentGoalId, const QString& currentTitle) {
    if (!m_goalHeaderLabel) {
        return;
    }

    if (currentGoalId != -1 && !currentTitle.isEmpty()) {
        // Timer is running with a goal
        m_goalHeaderLabel->setText(QString("%1").arg(currentTitle));
        m_goalHeaderLabel->setVisible(true);
    } else if (currentGoalId == -1) {
        // Quick timer is running
        m_goalHeaderLabel->setText("No Goal Selected");
        m_goalHeaderLabel->setVisible(true);
    } else {
        // Timer is not running
        m_goalHeaderLabel->setVisible(false);
    }
}

void TimerGoalsManager::updateTodayTimeSpent() {
    if (!m_todayTimeSpentLabel) {
        return;
    }

    DatabaseManager& db = DatabaseManager::instance();

    // Get today's total time in seconds
    int totalSeconds = db.getGoalTimeSpent(-1);

    // Convert to hours and minutes
    int hours = totalSeconds / 3600;
    int minutes = (totalSeconds % 3600) / 60;

    QString timeText;
    if (hours > 0) {
        timeText = QString("%1h %2m").arg(hours).arg(minutes);
    } else {
        timeText = QString("%1m").arg(minutes);
    }

    m_todayTimeSpentLabel->setText(QString("Today: %1").arg(timeText));
}

int TimerGoalsManager::currentSelectedGoalId() const {
    if (!m_goalSelectionCombo || m_goalSelectionCombo->currentIndex() < 0) {
        return -1;
    }
    return m_goalSelectionCombo->currentData().toInt();
}

void TimerGoalsManager::onGoalSelectionChanged() {
    if (!m_goalSelectionCombo) {
        return;
    }

    // Don't emit goal changes if this is called during UI refresh
    // Only emit when user actually changes selection manually
    int goalId = currentSelectedGoalId();
    QString title;
    QString description;

    if (goalId != -1) {
        // Get goal details from database
        DatabaseManager& db = DatabaseManager::instance();
        QList<DatabaseManager::GoalItem> goals = db.getAllGoals();

        for (const auto& goal : goals) {
            if (goal.id == goalId) {
                title = goal.title;
                description = goal.description;
                break;
            }
        }
    }

    // Only emit signal if this represents a real user selection change
    emit goalSelectionChanged(goalId, title, description);
}