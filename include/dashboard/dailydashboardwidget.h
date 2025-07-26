#ifndef DAILYDASHBOARDWIDGET_H
#define DAILYDASHBOARDWIDGET_H

#include "dashboard/basedashboardwidget.h"
#include "database/databasemanager.h"

class DailyDashboardWidget : public BaseDashboardWidget {
    Q_OBJECT

public:
    explicit DailyDashboardWidget(QWidget* parent = nullptr);

protected:
    void doUpdateDateDisplay() override;

    void doCalculateStats() override;

    bool doShouldIncludeTodo(const DatabaseManager::TodoItem& todo) const override;

    QString doGetWidgetTitle() const override;

 private:
    QString getOrdinalSuffix(int day) const;

};

#endif // DAILYDASHBOARDWIDGET_H