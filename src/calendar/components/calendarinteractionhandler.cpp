#include "calendar/components/calendarinteractionhandler.h"
#include <QCalendarWidget>
#include <QMouseEvent>
#include <QApplication>

CalendarInteractionHandler::CalendarInteractionHandler(QCalendarWidget* calendar, QObject* parent)
    : QObject(parent)
    , m_calendar(calendar)
{
}

bool CalendarInteractionHandler::handleMousePress(QMouseEvent* event)
{
    if (!isDetailsRequestEvent(event)) {
        return false;
    }

    QDate clickedDate = dateAt(event->pos());
    if (clickedDate.isValid()) {
        emit dayDetailsRequested(clickedDate);
        return true;
    }
    
    return false;
}

bool CalendarInteractionHandler::isDetailsRequestEvent(QMouseEvent* event) const
{
    return event->button() == Qt::RightButton ||
           (event->button() == Qt::LeftButton && QApplication::mouseButtons() & Qt::LeftButton);
}

QDate CalendarInteractionHandler::dateAt(const QPoint& pos) const
{
    return calculateDateFromPosition(pos);
}

QDate CalendarInteractionHandler::calculateDateFromPosition(const QPoint& pos) const
{
    if (!m_calendar) {
        return QDate();
    }

    QDate currentDate = m_calendar->selectedDate();
    int dayWidth = m_calendar->width() / DAYS_PER_WEEK;
    int dayHeight = (m_calendar->height() - APPROXIMATE_HEADER_HEIGHT) / WEEKS_IN_CALENDAR;

    int dayColumn = pos.x() / dayWidth;
    int dayRow = (pos.y() - APPROXIMATE_HEADER_HEIGHT) / dayHeight;

    QDate firstDay(currentDate.year(), currentDate.month(), 1);
    int firstDayOfWeek = firstDay.dayOfWeek() % 7;

    int dayNumber = (dayRow * DAYS_PER_WEEK + dayColumn) - firstDayOfWeek + 1;

    return (dayNumber >= 1 && dayNumber <= currentDate.daysInMonth())
           ? QDate(currentDate.year(), currentDate.month(), dayNumber)
           : QDate();
}