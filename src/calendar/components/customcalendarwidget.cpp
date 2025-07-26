#include "calendar/components/customcalendarwidget.h"
#include "calendar/components/daydetailsdialog.h"
#include "calendar/components/calendarinteractionhandler.h"
#include "calendar/components/calendarcellrenderer.h"
#include <QPainter>
#include <QMouseEvent>

CustomCalendarWidget::CustomCalendarWidget(QWidget* parent)
    : QCalendarWidget(parent)
    , m_detailsDialog(nullptr)
    , m_interactionHandler(nullptr)
{
    setupCalendar();
    setupInteractionHandler();
}

CustomCalendarWidget::~CustomCalendarWidget()
{
    // Qt handles cleanup through parent-child relationships
}

void CustomCalendarWidget::setupCalendar()
{
    setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
    setGridVisible(true);

    m_detailsDialog = new DayDetailsDialog(this);
}

void CustomCalendarWidget::setupInteractionHandler()
{
    m_interactionHandler = new CalendarInteractionHandler(this, this);
    connect(m_interactionHandler, &CalendarInteractionHandler::dayDetailsRequested,
            this, &CustomCalendarWidget::onDayDetailsRequested);
}

void CustomCalendarWidget::setDateGoalActivities(const QDate& date, const QList<GoalTimeInfo>& goalTimes)
{
    m_dateGoalActivities[date] = goalTimes;
    updateCell(date);
}

void CustomCalendarWidget::setDateTodos(const QDate& date, const QList<TodoInfo>& todos)
{
    m_dateTodos[date] = todos;
    updateCell(date);
}

void CustomCalendarWidget::clearActivities()
{
    m_dateGoalActivities.clear();
    m_dateTodos.clear();
    updateCells();
}

bool CustomCalendarWidget::hasContentForDate(const QDate& date) const
{
    return (!m_dateGoalActivities.value(date).isEmpty() ||
            !m_dateTodos.value(date).isEmpty());
}

void CustomCalendarWidget::paintCell(QPainter* painter, const QRect& rect, QDate date) const
{
    // First paint the default calendar cell
    QCalendarWidget::paintCell(painter, rect, date);

    // Then add our custom content if available
    if (hasContentForDate(date)) {
        QList<GoalTimeInfo> goals = m_dateGoalActivities.value(date);
        QList<TodoInfo> todos = m_dateTodos.value(date);

        CalendarCellRenderer::renderCell(painter, rect, date, goals, todos);
    }
}

void CustomCalendarWidget::mousePressEvent(QMouseEvent* event)
{
    // Try to handle the event with our interaction handler first
    if (m_interactionHandler && m_interactionHandler->handleMousePress(event)) {
        return; // Event was handled
    }

    // Fall back to default behavior
    QCalendarWidget::mousePressEvent(event);
}

void CustomCalendarWidget::onDayDetailsRequested(const QDate& date)
{
    if (!hasContentForDate(date)) {
        return;
    }

    QList<GoalTimeInfo> goals = m_dateGoalActivities.value(date);
    QList<TodoInfo> todos = m_dateTodos.value(date);

    m_detailsDialog->setData(date, goals, todos);
    m_detailsDialog->exec();
}