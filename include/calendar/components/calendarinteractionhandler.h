#ifndef CALENDARINTERACTIONHANDLER_H
#define CALENDARINTERACTIONHANDLER_H

#include <QObject>
#include <QPoint>
#include <QDate>

class QCalendarWidget;
class QMouseEvent;

class CalendarInteractionHandler : public QObject {
    Q_OBJECT

public:
    explicit CalendarInteractionHandler(QCalendarWidget* calendar, QObject* parent = nullptr);

    bool handleMousePress(QMouseEvent* event);
    QDate dateAt(const QPoint& pos) const;

    signals:
        void dayDetailsRequested(const QDate& date);

private:
    bool isDetailsRequestEvent(QMouseEvent* event) const;
    QDate calculateDateFromPosition(const QPoint& pos) const;

    QCalendarWidget* m_calendar;

    // Layout constants for date calculation
    static constexpr int APPROXIMATE_HEADER_HEIGHT = 50;
    static constexpr int DAYS_PER_WEEK = 7;
    static constexpr int WEEKS_IN_CALENDAR = 6;
};

#endif // CALENDARINTERACTIONHANDLER_H