#ifndef CALENDARWIDGET_H
#define CALENDARWIDGET_H

#include <QWidget>
#include <QDate>
#include <QList>
#include "database/databasemanager.h"

class QVBoxLayout;
class QHBoxLayout;
class QLabel;
class QPushButton;
class QListWidget;
class CustomCalendarWidget;
class CalendarNavigation;

class CalendarWidget : public QWidget {
    Q_OBJECT

public:
    explicit CalendarWidget(QWidget* parent = nullptr);

public slots:
    void refreshFromDatabase();

private slots:
    void onDateSelected(const QDate& date);
    void onPreviousMonth();
    void onNextMonth();
    void onTodayClicked();
    void onMonthSelected(int month);
    void onYearSelected(int year);

private:
    // Constants
    static const QString NO_GOAL_TITLE;
    static const QString NO_GOAL_COLOR;

    // Setup methods
    void setupUi();
    void setupMainHeader();
    void setupCalendarSection();
    void setupDetailsPanel();
    void connectSignals();

    // Data processing methods
    void updateDateDetails(const QDate& date);
    void updateCalendar();
    void updateCalendarWithGoalData();
    void updateCalendarWithTodoData();

    // Helper methods
    void navigateToDate(const QDate& date);
    void populateActivitiesList(const QDate& date);
    void addGoalSummariesToList(const QDate& date);
    void addTodosToList(const QDate& date);

    // UI Components
    QVBoxLayout* m_mainLayout;
    CustomCalendarWidget* m_calendar;
    CalendarNavigation* m_navigation;

    // Details panel
    QWidget* m_detailsPanel;
    QVBoxLayout* m_detailsLayout;
    QLabel* m_selectedDateLabel;
    QLabel* m_totalTimeLabel;
    QListWidget* m_activitiesList;

    // Data
    QList<DatabaseManager::TimerRecord> m_timerRecords;
    QList<DatabaseManager::TodoItem> m_todos;
};

#endif // CALENDARWIDGET_H