#include "dashboard/weeklydashboardwidget.h"

#include <QDate>
#include <QDateTime>
#include <QTime>

WeeklyDashboardWidget::WeeklyDashboardWidget(QWidget* parent)
    : BaseDashboardWidget(parent)
    , m_weekStart()
    , m_weekEnd()
{
    // Apply weekly-specific modern styling with subtle differences
    setStyleSheet(
        "WeeklyDashboardWidget {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "       stop:0 #FFFAFA, stop:0.3 #FFF6F6, stop:0.7 #FFF3F3, stop:1 #FFF0F0);"
        "   border-radius: 20px;"
        "   border: 2px solid #FFD0D0;"
        "   margin: 5px;"
        "}"
    );

    updateWeekRange();
    updateTitle();
    refreshFromDatabase();
}

void WeeklyDashboardWidget::updateWeekRange()
{
    const QDate today = QDate::currentDate();

    // Calculate week start (Monday) and end (Sunday)
    // Qt's dayOfWeek(): Monday = 1, Sunday = 7
    m_weekStart = today.addDays(-(today.dayOfWeek() - 1));
    m_weekEnd = m_weekStart.addDays(6);
}

void WeeklyDashboardWidget::doUpdateDateDisplay()
{
    if (m_isBeingDestroyed || !m_dateLabel) {
        return;
    }

    updateWeekRange();

    // Hide the date label since we're using the title now
    m_dateLabel->setVisible(false);

    // Update todo list headers with localized strings
    if (m_pendingTodosListLabel) {
        m_pendingTodosListLabel->setText(WEEK_PENDING_TODOS_TITLE);
    }

    if (m_completedTodosListLabel) {
        m_completedTodosListLabel->setText(WEEK_COMPLETED_TODOS_TITLE);
    }
}

void WeeklyDashboardWidget::doCalculateStats()
{
    if (m_isBeingDestroyed) {
        return;
    }

    updateWeekRange();

    // Calculate this week's time spent across all goals
    m_timeSpentSeconds = 0;
    const auto allGoals = DatabaseManager::instance().getAllGoals(true);
    for (const auto& goal : allGoals) {
        m_timeSpentSeconds += DatabaseManager::instance().getGoalTimeSpentInPeriod(
            goal.id, m_weekStart, m_weekEnd);
    }

    // Get this week's completed timer sessions
    const QDateTime startOfWeek(m_weekStart, QTime(0, 0));
    const QDateTime endOfWeek(m_weekEnd, QTime(23, 59, 59));
    const auto timerRecords = DatabaseManager::instance().getTimerRecords(startOfWeek, endOfWeek);
    m_timersCompletedCount = timerRecords.size();

    // Filter and count this week's todos - handled by base class using shouldIncludeTodo()
}

bool WeeklyDashboardWidget::doShouldIncludeTodo(const DatabaseManager::TodoItem& todo) const
{
    if (m_isBeingDestroyed) {
        return false;
    }

    // Include todos that overlap with the current week
    // Week range is already calculated: m_weekStart to m_weekEnd

    bool isInWeekRange = false;

    if (!todo.startDate.isValid() && !todo.endDate.isValid()) {
        // No specific dates - consider as general todo for current week
        isInWeekRange = true;
    } else if (todo.startDate.isValid() && !todo.endDate.isValid()) {
        // Only start date - include if start date is within or before this week
        isInWeekRange = (todo.startDate <= m_weekEnd);
    } else if (!todo.startDate.isValid() && todo.endDate.isValid()) {
        // Only end date - include if end date is within or after this week
        isInWeekRange = (todo.endDate >= m_weekStart);
    } else if (todo.startDate.isValid() && todo.endDate.isValid()) {
        // Both dates set - include if there's any overlap with this week
        // Overlap exists if: todo.startDate <= m_weekEnd && todo.endDate >= m_weekStart
        isInWeekRange = (todo.startDate <= m_weekEnd && todo.endDate >= m_weekStart);
    }

    return isInWeekRange;
}


QString WeeklyDashboardWidget::getOrdinalSuffix(int day) const
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

QString WeeklyDashboardWidget::doGetWidgetTitle() const
{
    // Calculate current week range
    const QDate today = QDate::currentDate();
    QDate weekStart = today.addDays(-(today.dayOfWeek() - 1));
    QDate weekEnd = weekStart.addDays(6);

    // Format the week range display elegantly with ordinal suffixes
    QString dateRange;
    if (weekStart.month() == weekEnd.month()) {
        // Same month: "March 15th - 21st, 2024"
        dateRange = QString("%1 %2%3 - %4%5, %6")
                   .arg(weekStart.toString("MMMM"))
                   .arg(weekStart.day())
                   .arg(getOrdinalSuffix(weekStart.day()))
                   .arg(weekEnd.day())
                   .arg(getOrdinalSuffix(weekEnd.day()))
                   .arg(weekEnd.year());
    } else if (weekStart.year() == weekEnd.year()) {
        // Different months, same year: "March 29th - April 4th, 2024"
        dateRange = QString("%1 %2%3 - %4 %5%6, %7")
                   .arg(weekStart.toString("MMMM"))
                   .arg(weekStart.day())
                   .arg(getOrdinalSuffix(weekStart.day()))
                   .arg(weekEnd.toString("MMMM"))
                   .arg(weekEnd.day())
                   .arg(getOrdinalSuffix(weekEnd.day()))
                   .arg(weekEnd.year());
    } else {
        // Different years: "December 29th, 2023 - January 4th, 2024"
        dateRange = QString("%1 %2%3, %4 - %5 %6%7, %8")
                   .arg(weekStart.toString("MMMM"))
                   .arg(weekStart.day())
                   .arg(getOrdinalSuffix(weekStart.day()))
                   .arg(weekStart.year())
                   .arg(weekEnd.toString("MMMM"))
                   .arg(weekEnd.day())
                   .arg(getOrdinalSuffix(weekEnd.day()))
                   .arg(weekEnd.year());
    }

    return QString("This Week&nbsp;&nbsp;<span style='font-size: 18px; font-weight: 500;'> %1</span>").arg(dateRange);
}