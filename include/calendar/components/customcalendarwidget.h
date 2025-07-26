#ifndef CUSTOMCALENDARWIDGET_H
#define CUSTOMCALENDARWIDGET_H

#include <QCalendarWidget>
#include <QColor>
#include <QMap>
#include <QDate>
#include <QList>
#include "database/databasemanager.h"

struct GoalTimeInfo {
    int goalId;
    QString goalTitle;
    QColor goalColor;
    int minutes;
};

struct TodoInfo {
    int todoId;
    QString todoTitle;
    DatabaseManager::TodoPriority priority;
    bool isCompleted;
};

class DayDetailsDialog;
class CalendarInteractionHandler;

class CustomCalendarWidget : public QCalendarWidget {
    Q_OBJECT

public:
    explicit CustomCalendarWidget(QWidget* parent = nullptr);
    ~CustomCalendarWidget();

    void setDateGoalActivities(const QDate& date, const QList<GoalTimeInfo>& goalTimes);
    void setDateTodos(const QDate& date, const QList<TodoInfo>& todos);
    void clearActivities();

protected:
    void paintCell(QPainter* painter, const QRect& rect, QDate date) const;
    void mousePressEvent(QMouseEvent* event) override;

private slots:
    void onDayDetailsRequested(const QDate& date);

private:
    void setupCalendar();
    void setupInteractionHandler();
    bool hasContentForDate(const QDate& date) const;

    // Data storage
    QMap<QDate, QList<GoalTimeInfo>> m_dateGoalActivities;
    QMap<QDate, QList<TodoInfo>> m_dateTodos;

    // UI Components
    DayDetailsDialog* m_detailsDialog;
    CalendarInteractionHandler* m_interactionHandler;
};

#endif // CUSTOMCALENDARWIDGET_H