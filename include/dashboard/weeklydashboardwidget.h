#ifndef WEEKLYDASHBOARDWIDGET_H
#define WEEKLYDASHBOARDWIDGET_H

#include <QDate>

#include "dashboard/basedashboardwidget.h"
#include "database/databasemanager.h"

class WeeklyDashboardWidget : public BaseDashboardWidget    {
    Q_OBJECT

public:
    explicit WeeklyDashboardWidget(QWidget* parent = nullptr);

protected:
    void doUpdateDateDisplay() override;

    void doCalculateStats() override;

    bool doShouldIncludeTodo(const DatabaseManager::TodoItem& todo) const override;

    QString doGetWidgetTitle() const override;

private:
    void updateWeekRange();

    QString getOrdinalSuffix(int day) const;

    // Week date range
    QDate m_weekStart;
    QDate m_weekEnd;
};

#endif // WEEKLYDASHBOARDWIDGET_H