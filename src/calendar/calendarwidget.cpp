#include "calendar/calendarwidget.h"
#include "calendar/components/customcalendarwidget.h"
#include "calendar/components/calendarnavigation.h"
#include "calendar/components/coloreditemdelegate.h"
#include "calendar/components/calendarstyles.h"
#include "calendar/components/calendarutils.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QFont>
#include <QTime>
#include <QDate>

const QString CalendarWidget::NO_GOAL_TITLE = "No Goal";
const QString CalendarWidget::NO_GOAL_COLOR = "#C0C0C0";

CalendarWidget::CalendarWidget(QWidget* parent)
    : QWidget(parent)
    , m_mainLayout(nullptr)
    , m_calendar(nullptr)
    , m_navigation(nullptr)
    , m_detailsPanel(nullptr)
    , m_detailsLayout(nullptr)
    , m_selectedDateLabel(nullptr)
    , m_totalTimeLabel(nullptr)
    , m_activitiesList(nullptr)
{
    setupUi();
    updateCalendar();
    refreshFromDatabase();
}

void CalendarWidget::setupUi()
{
    setStyleSheet(CalendarStyles::MAIN_WIDGET_STYLE);

    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(20, 20, 20, 20);
    m_mainLayout->setSpacing(15);

    setupMainHeader();
    setupCalendarSection();
    connectSignals();

    // Initial setup
    m_navigation->updateDisplay(QDate::currentDate());
    onDateSelected(QDate::currentDate());
}

void CalendarWidget::setupMainHeader()
{
    QWidget* headerWidget = new QWidget(this);
    headerWidget->setFixedHeight(80);
    headerWidget->setStyleSheet(CalendarStyles::HEADER_GRADIENT_STYLE);

    QHBoxLayout* headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->setContentsMargins(20, 0, 0, 0);
    headerLayout->setSpacing(20);

    QLabel* titleLabel = new QLabel("Calendar", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(24);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setStyleSheet(QString("QLabel { color: %1; border: none; }")
                             .arg(CalendarStyles::Colors::TEXT_ACCENT));

    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch();

    m_mainLayout->addWidget(headerWidget);
}

void CalendarWidget::setupCalendarSection()
{
    // Content area with calendar and details
    QWidget* contentWidget = new QWidget(this);
    QHBoxLayout* contentLayout = new QHBoxLayout(contentWidget);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(15);

    // Calendar section
    QWidget* calendarSection = new QWidget(this);
    QVBoxLayout* calendarLayout = new QVBoxLayout(calendarSection);
    calendarLayout->setContentsMargins(0, 0, 0, 0);
    calendarLayout->setSpacing(10);

    // Navigation
    m_navigation = new CalendarNavigation(this);
    calendarLayout->addWidget(m_navigation);

    // Calendar widget
    m_calendar = new CustomCalendarWidget(this);
    m_calendar->setMinimumSize(500, 400);
    m_calendar->setStyleSheet(CalendarStyles::getCalendarStyle());
    calendarLayout->addWidget(m_calendar);

    contentLayout->addWidget(calendarSection, 2);

    // Details panel
    setupDetailsPanel();
    contentLayout->addWidget(m_detailsPanel, 1);

    m_mainLayout->addWidget(contentWidget);
}

void CalendarWidget::setupDetailsPanel()
{
    m_detailsPanel = new QWidget(this);
    m_detailsPanel->setStyleSheet(CalendarStyles::getDetailsStyle());
    m_detailsPanel->setMinimumWidth(300);
    m_detailsPanel->setMaximumWidth(380);

    m_detailsLayout = new QVBoxLayout(m_detailsPanel);
    m_detailsLayout->setContentsMargins(20, 20, 20, 20);
    m_detailsLayout->setSpacing(5);

    // Selected date header
    m_selectedDateLabel = new QLabel("Select a date", this);
    QFont dateFont = m_selectedDateLabel->font();
    dateFont.setPointSize(16);
    dateFont.setBold(true);
    m_selectedDateLabel->setFont(dateFont);
    m_selectedDateLabel->setAlignment(Qt::AlignCenter);
    m_selectedDateLabel->setStyleSheet(QString(
        "QLabel {"
        "   color: %1;"
        "   background-color: rgba(255, 255, 255, 0.7);"
        "   border-radius: 8px;"
        "   padding: 12px;"
        "   border: 1px solid %2;"
        "}"
    ).arg(CalendarStyles::Colors::TEXT_SECONDARY, CalendarStyles::Colors::BORDER_LIGHT));
    m_detailsLayout->addWidget(m_selectedDateLabel);

    // Total time label
    m_totalTimeLabel = new QLabel("Total time: 0h 0m", this);
    QFont timeFont = m_totalTimeLabel->font();
    timeFont.setPointSize(12);
    timeFont.setBold(true);
    m_totalTimeLabel->setFont(timeFont);
    m_totalTimeLabel->setAlignment(Qt::AlignCenter);
    m_totalTimeLabel->setStyleSheet(QString(
        "QLabel {"
        "   color: %1;"
        "   background-color: rgba(255, 179, 179, 0.3);"
        "   border-radius: 6px;"
        "   padding: 8px;"
        "}"
    ).arg(CalendarStyles::Colors::TEXT_ACCENT));
    m_detailsLayout->addWidget(m_totalTimeLabel);

    // Activities list
    QLabel* activitiesHeader = new QLabel("Activities", this);
    QFont headerFont = activitiesHeader->font();
    headerFont.setPointSize(14);
    headerFont.setBold(true);
    activitiesHeader->setFont(headerFont);
    activitiesHeader->setStyleSheet(QString(
        "QLabel {"
        "   color: %1;"
        "   background-color: transparent;"
        "   border: none;"
        "}"
    ).arg(CalendarStyles::Colors::TEXT_SECONDARY));
    m_detailsLayout->addWidget(activitiesHeader);

    m_activitiesList = new QListWidget(this);
    m_activitiesList->setItemDelegate(new ColoredItemDelegate(this));
    m_activitiesList->setStyleSheet(CalendarStyles::getListStyle());
    m_detailsLayout->addWidget(m_activitiesList, 1);
    m_detailsLayout->addStretch();
}

void CalendarWidget::connectSignals()
{
    connect(m_navigation, &CalendarNavigation::previousMonthClicked, this, &CalendarWidget::onPreviousMonth);
    connect(m_navigation, &CalendarNavigation::nextMonthClicked, this, &CalendarWidget::onNextMonth);
    connect(m_navigation, &CalendarNavigation::todayClicked, this, &CalendarWidget::onTodayClicked);
    connect(m_navigation, &CalendarNavigation::monthSelected, this, &CalendarWidget::onMonthSelected);
    connect(m_navigation, &CalendarNavigation::yearSelected, this, &CalendarWidget::onYearSelected);

    connect(m_calendar, &QCalendarWidget::selectionChanged, this, [this]() {
        onDateSelected(m_calendar->selectedDate());
    });
}

void CalendarWidget::updateDateDetails(const QDate& date)
{
    m_selectedDateLabel->setText(CalendarUtils::formatDate(date));

    int totalSeconds = 0;
    for (const auto& record : m_timerRecords) {
        if (record.startTime.date() == date) {
            totalSeconds += record.startTime.secsTo(record.endTime);
        }
    }

    m_totalTimeLabel->setText(QString("Total time: %1")
                             .arg(CalendarUtils::formatDuration(totalSeconds)));

    populateActivitiesList(date);
}

void CalendarWidget::populateActivitiesList(const QDate& date)
{
    m_activitiesList->clear();

    addGoalSummariesToList(date);
    addTodosToList(date);

    if (m_activitiesList->count() == 0) {
        QListWidgetItem* item = new QListWidgetItem("No activities recorded");
        item->setTextAlignment(Qt::AlignCenter);
        item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
        m_activitiesList->addItem(item);
    }
}

void CalendarWidget::addGoalSummariesToList(const QDate& date)
{
    QMap<int, int> goalTotalTime;
    QMap<int, DatabaseManager::GoalItem> goalInfo;

    // Calculate goal time for the date
    for (const auto& record : m_timerRecords) {
        if (record.startTime.date() == date) {
            goalTotalTime[record.goalId] += record.startTime.secsTo(record.endTime);
            if (goalInfo.find(record.goalId) == goalInfo.end()) {
                goalInfo[record.goalId] = DatabaseManager::instance().getGoal(record.goalId);
            }
        }
    }

    if (!goalTotalTime.isEmpty()) {
        // Header for goals
        QListWidgetItem* goalHeader = new QListWidgetItem("Goal Time Summary");
        QFont headerFont = goalHeader->font();
        headerFont.setBold(true);
        headerFont.setPointSize(12);
        goalHeader->setFont(headerFont);
        goalHeader->setFlags(goalHeader->flags() & ~Qt::ItemIsSelectable);
        goalHeader->setBackground(QColor(CalendarStyles::Colors::GRADIENT_END));
        m_activitiesList->addItem(goalHeader);

        for (auto it = goalTotalTime.constBegin(); it != goalTotalTime.constEnd(); ++it) {
            int goalId = it.key();
            int totalTime = it.value();
            DatabaseManager::GoalItem goal = goalInfo[goalId];

            QString goalTitle = goal.id != -1 ? goal.title : NO_GOAL_TITLE;
            QString colorCode = goal.id != -1 ? goal.colorCode : NO_GOAL_COLOR;

            QString itemText = QString("%1 - %2")
                              .arg(goalTitle)
                              .arg(CalendarUtils::formatDuration(totalTime));

            QListWidgetItem* item = new QListWidgetItem(itemText);
            item->setData(Qt::UserRole, goalId);
            item->setData(Qt::UserRole + 1, colorCode);
            item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
            m_activitiesList->addItem(item);
        }
    }
}

void CalendarWidget::addTodosToList(const QDate& date)
{
    QList<DatabaseManager::TodoItem> dayTodos;
    for (const auto& todo : m_todos) {
        if ((todo.startDate.isValid() && todo.startDate == date) ||
            (todo.endDate.isValid() && todo.endDate == date)) {
            dayTodos.append(todo);
        }
    }

    if (!dayTodos.isEmpty()) {
        QListWidgetItem* todoHeader = new QListWidgetItem("To-dos");
        QFont headerFont = todoHeader->font();
        headerFont.setBold(true);
        headerFont.setPointSize(12);
        todoHeader->setFont(headerFont);
        todoHeader->setFlags(todoHeader->flags() & ~Qt::ItemIsSelectable);
        todoHeader->setBackground(QColor(CalendarStyles::Colors::GRADIENT_END));
        m_activitiesList->addItem(todoHeader);

        for (const auto& todo : dayTodos) {
            QListWidgetItem* item = new QListWidgetItem(todo.title);
            if (todo.isCompleted) {
                QFont font = item->font();
                font.setStrikeOut(true);
                item->setFont(font);
                item->setForeground(QColor::fromRgb(128, 128, 128));
            }
            item->setData(Qt::UserRole, todo.id);
            item->setData(Qt::UserRole + 2, "todo");
            item->setData(Qt::UserRole + 3, todo.isCompleted);
            item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
            m_activitiesList->addItem(item);
        }
    }
}

void CalendarWidget::updateCalendar()
{
    if (!m_calendar) return;

    m_calendar->clearActivities();
    updateCalendarWithGoalData();
    updateCalendarWithTodoData();
}

void CalendarWidget::updateCalendarWithGoalData()
{
    QMap<QDate, QList<GoalTimeInfo>> dateGoalActivities;
    QMap<QPair<QDate, int>, int> goalTimePerDay;

    // Calculate goal time per day
    for (const auto& record : m_timerRecords) {
        QDate recordDate = record.startTime.date();
        int duration = record.startTime.secsTo(record.endTime);
        QPair<QDate, int> key(recordDate, record.goalId);
        goalTimePerDay[key] += duration;
    }

    // Create goal time info for calendar
    for (auto it = goalTimePerDay.constBegin(); it != goalTimePerDay.constEnd(); ++it) {
        QDate date = it.key().first;
        int goalId = it.key().second;
        int totalSeconds = it.value();

        DatabaseManager::GoalItem goal = DatabaseManager::instance().getGoal(goalId);

        GoalTimeInfo goalInfo;
        goalInfo.goalId = goalId;
        goalInfo.goalTitle = goal.id != -1 ? goal.title : NO_GOAL_TITLE;
        goalInfo.goalColor = QColor(goal.id != -1 ? goal.colorCode : NO_GOAL_COLOR);
        goalInfo.minutes = totalSeconds / 60;

        dateGoalActivities[date].append(goalInfo);
    }

    // Update calendar
    for (auto it = dateGoalActivities.constBegin(); it != dateGoalActivities.constEnd(); ++it) {
        m_calendar->setDateGoalActivities(it.key(), it.value());
    }
}

void CalendarWidget::updateCalendarWithTodoData()
{
    QMap<QDate, QList<TodoInfo>> dateTodos;

    for (const auto& todo : m_todos) {
        if (todo.startDate.isValid()) {
            TodoInfo todoInfo;
            todoInfo.todoId = todo.id;
            todoInfo.todoTitle = todo.title;
            todoInfo.priority = todo.priority;
            todoInfo.isCompleted = todo.isCompleted;
            dateTodos[todo.startDate].append(todoInfo);
        }

        if (todo.endDate.isValid() && todo.endDate != todo.startDate) {
            TodoInfo todoInfo;
            todoInfo.todoId = todo.id;
            todoInfo.todoTitle = todo.title;
            todoInfo.priority = todo.priority;
            todoInfo.isCompleted = todo.isCompleted;
            dateTodos[todo.endDate].append(todoInfo);
        }
    }

    for (auto it = dateTodos.constBegin(); it != dateTodos.constEnd(); ++it) {
        m_calendar->setDateTodos(it.key(), it.value());
    }
}

void CalendarWidget::navigateToDate(const QDate& date)
{
    m_calendar->setSelectedDate(date);
    m_navigation->updateDisplay(date);
    refreshFromDatabase();
}

void CalendarWidget::onDateSelected(const QDate& date)
{
    updateDateDetails(date);
}

void CalendarWidget::onPreviousMonth()
{
    QDate currentDate = m_calendar->selectedDate();
    navigateToDate(currentDate.addMonths(-1));
}

void CalendarWidget::onNextMonth()
{
    QDate currentDate = m_calendar->selectedDate();
    navigateToDate(currentDate.addMonths(1));
}

void CalendarWidget::onTodayClicked()
{
    navigateToDate(QDate::currentDate());
}

void CalendarWidget::onMonthSelected(int month)
{
    QDate currentDate = m_calendar->selectedDate();
    QDate newDate(currentDate.year(), month, 1);
    navigateToDate(newDate);
}

void CalendarWidget::onYearSelected(int year)
{
    QDate currentDate = m_calendar->selectedDate();
    QDate newDate(year, currentDate.month(), 1);
    navigateToDate(newDate);
}

void CalendarWidget::refreshFromDatabase()
{
    QDate startOfMonth = m_calendar->selectedDate();
    startOfMonth = QDate(startOfMonth.year(), startOfMonth.month(), 1);
    QDate endOfMonth = startOfMonth.addMonths(1).addDays(-1);

    QDateTime startDateTime(startOfMonth, QTime(0, 0, 0));
    QDateTime endDateTime(endOfMonth, QTime(23, 59, 59));

    m_timerRecords = DatabaseManager::instance().getTimerRecords(startDateTime, endDateTime);
    m_todos = DatabaseManager::instance().getAllTodos(true);

    updateCalendar();
    updateDateDetails(m_calendar->selectedDate());
}