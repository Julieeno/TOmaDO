
#include "dashboard/dailydashboardwidget.h"

#include <QDate>
#include <QDateTime>
#include <QTime>

DailyDashboardWidget::DailyDashboardWidget(QWidget* parent)
    : BaseDashboardWidget(parent)
{
    // Apply daily-specific modern styling
    setStyleSheet(
        "DailyDashboardWidget {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "       stop:0 #FFFBFB, stop:0.3 #FFF8F8, stop:0.7 #FFF5F5, stop:1 #FFF2F2);"
        "   border-radius: 20px;"
        "   border: 2px solid #FFD6D6;"
        "   margin: 5px;"
        "}"
    );

    updateTitle();
    refreshFromDatabase();
}

void DailyDashboardWidget::doUpdateDateDisplay()
{
    if (m_isBeingDestroyed || !m_dateLabel) {
        return;
    }

    // Hide the date label since we're using the title now
    m_dateLabel->setVisible(false);

    // Update todo list headers with localized strings
    if (m_pendingTodosListLabel) {
        m_pendingTodosListLabel->setText(TODAY_PENDING_TODOS_TITLE);
    }

    if (m_completedTodosListLabel) {
        m_completedTodosListLabel->setText(TODAY_COMPLETED_TODOS_TITLE);
    }
}

void DailyDashboardWidget::doCalculateStats()
{
    if (m_isBeingDestroyed) {
        return;
    }

    const QDate today = QDate::currentDate();

    // Calculate today's time spent across all goals with better performance
    m_timeSpentSeconds = 0;
    const auto allGoals = DatabaseManager::instance().getAllGoals(true);
    for (const auto& goal : allGoals) {
        m_timeSpentSeconds += DatabaseManager::instance().getGoalTimeSpentInPeriod(
            goal.id, today, today);
    }

    // Get today's completed timer sessions efficiently
    const QDateTime startOfDay(today, QTime(0, 0));
    const QDateTime endOfDay(today, QTime(23, 59, 59));
    const auto timerRecords = DatabaseManager::instance().getTimerRecords(startOfDay, endOfDay);
    m_timersCompletedCount = timerRecords.size();

    // Filter and count today's todos - already done in base class calculateStats()
    // The base class handles the filtering using shouldIncludeTodo()
}

bool DailyDashboardWidget::doShouldIncludeTodo(const DatabaseManager::TodoItem& todo) const
{
    if (m_isBeingDestroyed) {
        return false;
    }

    const QDate today = QDate::currentDate();

    bool isInDateRange = false;

    if (!todo.startDate.isValid() && !todo.endDate.isValid()) {
        // No specific dates - consider as general todo for today
        isInDateRange = true;
    } else if (todo.startDate.isValid() && !todo.endDate.isValid()) {
        // Only start date - include if start date is today or before
        isInDateRange = (todo.startDate <= today);
    } else if (!todo.startDate.isValid() && todo.endDate.isValid()) {
        // Only end date - include if end date is today or after
        isInDateRange = (todo.endDate >= today);
    } else if (todo.startDate.isValid() && todo.endDate.isValid()) {
        // Both dates set - include if today is within range
        isInDateRange = (todo.startDate <= today && today <= todo.endDate);
    }

    return isInDateRange;
}

QString DailyDashboardWidget::getOrdinalSuffix(int day) const
{
    if (day >= 11 && day <= 13) {
        return "th";  // Special case for 11th, 12th, 13th
    }

    switch (day % 10) {
        case 1: return "st";
        case 2: return "nd";
        case 3: return "rd";
        default: return "th";
    }
}

QString DailyDashboardWidget::doGetWidgetTitle() const
{
    const QDate today = QDate::currentDate();
    QString formattedDate = QString("%1, %2 %3%4, %5")
                           .arg(today.toString("dddd"))
                           .arg(today.toString("MMMM"))
                           .arg(today.day())
                           .arg(getOrdinalSuffix(today.day()))
                           .arg(today.year());

    return QString("Today&nbsp;&nbsp;<span style='font-size: 18px; font-weight: 500;'>%1</span>").arg(formattedDate);
}