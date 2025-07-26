
#include "dashboard/totaldashboardwidget.h"

#include <QDateTime>
#include <QTime>

TotalDashboardWidget::TotalDashboardWidget(QWidget* parent)
    : BaseDashboardWidget(parent)
{
    // Apply total-specific modern styling with distinctive appearance
    setStyleSheet(
        "TotalDashboardWidget {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "       stop:0 #FFFEFE, stop:0.2 #FFFAFA, stop:0.5 #FFF7F7, stop:0.8 #FFF4F4, stop:1 #FFF1F1);"
        "   border-radius: 20px;"
        "   border: 2px solid #FFCACA;"
        "   margin: 5px;"
        "}"
    );

    updateTitle();
    refreshFromDatabase();
}

void TotalDashboardWidget::doUpdateDateDisplay()
{
    if (m_isBeingDestroyed || !m_dateLabel) {
        return;
    }

    // Hide the date label since we're using the title now
    m_dateLabel->setVisible(false);

    // Update todo list headers with localized strings
    if (m_pendingTodosListLabel) {
        m_pendingTodosListLabel->setText(TOTAL_PENDING_TODOS_TITLE);
    }

    if (m_completedTodosListLabel) {
        m_completedTodosListLabel->setText(TOTAL_COMPLETED_TODOS_TITLE);
    }
}

void TotalDashboardWidget::doCalculateStats()
{
    if (m_isBeingDestroyed) {
        return;
    }

    // Cache database instance to avoid repeated calls
    DatabaseManager& db = DatabaseManager::instance();

    // Calculate total time spent across all goals and all time
    m_timeSpentSeconds = 0;
    const auto allGoals = db.getAllGoals(true);

    // Use a very wide date range to capture all historical data
    const QDate earliestDate(2000, 1, 1);
    const QDate latestDate(2100, 12, 31);

    for (const auto& goal : allGoals) {
        m_timeSpentSeconds += db.getGoalTimeSpentInPeriod(
            goal.id, earliestDate, latestDate);
    }

    // Get all completed timer sessions using a very wide date range
    const QDateTime startDate(earliestDate, QTime(0, 0));
    const QDateTime endDate(latestDate, QTime(23, 59, 59));
    const auto allTimerRecords = db.getTimerRecords(startDate, endDate);
    m_timersCompletedCount = allTimerRecords.size();

    // Filter and count all todos - handled by base class using shouldIncludeTodo()
    // For total dashboard, we include everything, so shouldIncludeTodo returns true for all
}

bool TotalDashboardWidget::doShouldIncludeTodo(const DatabaseManager::TodoItem& /*todo*/) const
{
    if (m_isBeingDestroyed) {
        return false;
    }

    // For total dashboard, include all todos regardless of dates
    return true;
}

QString TotalDashboardWidget::doGetWidgetTitle() const
{
    return tr("All Time Overview");
}