#ifndef TOTALDASHBOARDWIDGET_H
#define TOTALDASHBOARDWIDGET_H

#include "dashboard/basedashboardwidget.h"
#include "database/databasemanager.h"

class TotalDashboardWidget : public BaseDashboardWidget {
    Q_OBJECT

public:
    explicit TotalDashboardWidget(QWidget* parent = nullptr);

protected:
    void doUpdateDateDisplay() override;

    void doCalculateStats() override;

    bool doShouldIncludeTodo(const DatabaseManager::TodoItem& todo) const override;

    QString doGetWidgetTitle() const override;
};

#endif // TOTALDASHBOARDWIDGET_H