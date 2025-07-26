#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QDateTime>
#include <QString>
#include <QList>
#include <QDate>
#include <memory>

class GoalRepository;
class TodoRepository;
class TimerRepository;
class ColorService;

class DatabaseManager : public QObject {
    Q_OBJECT

public:
    enum class TodoPriority {
        Low,
        Medium,
        High
    };

    struct TodoItem {
        int id = -1;
        QString title;
        QString description;
        TodoPriority priority = TodoPriority::Medium;
        bool isCompleted = false;
        int goalId = -1;
        QString colorCode;
        QDate startDate;
        QDate endDate;
        QDateTime lastUpdate;
    };

    struct GoalItem {
        int id = -1;
        QString title;
        QString description;
        TodoPriority priority = TodoPriority::Medium;
        bool isCompleted = false;
        int estimatedTime = 0;  // in seconds
        QString colorCode;
    };

    struct TimerRecord {
        int id = -1;
        QDateTime startTime;
        QDateTime endTime;
        int goalId = -1;
    };

    // Singleton access
    static DatabaseManager& instance();

    // Initialization
    bool initialize();
    bool isInitialized() const;

    // Timer records operations
    bool addTimerRecord(const TimerRecord& record);
    TimerRecord getTimerRecord(int id);
    QList<TimerRecord> getTimerRecords(const QDateTime& start, const QDateTime& end);
    bool clearTimerRecords();

    // Goals operations
    bool addGoal(const GoalItem& goal);
    bool updateGoal(const GoalItem& goal);
    bool deleteGoal(int id);
    GoalItem getGoal(int id);
    QList<GoalItem> getAllGoals(bool includeCompleted = true);
    bool toggleGoalCompletion(int id, bool completed);
    bool completeGoal(int goalId);

    // Goal time tracking
    int getGoalTimeSpent(int goalId);
    int getGoalTimeSpentInPeriod(int goalId, const QDate& startDate, const QDate& endDate);

    // Todos operations
    bool addTodo(const TodoItem& todo);
    bool updateTodo(const TodoItem& todo);
    bool deleteTodo(int id);
    TodoItem getTodo(int id);
    QList<TodoItem> getAllTodos(bool includeCompleted = true);
    bool toggleTodoCompletion(int id, bool completed);
    QList<TodoItem> getTodosByDateRange(const QDate& startDate, const QDate& endDate, bool includeCompleted = true);
    bool clearTodos();

    // Colors operations
    QString getColorName(const QString& colorCode);
    QStringList getAvailableColors();

signals:
    void dataChanged();

private:
    DatabaseManager();
    ~DatabaseManager();

    // Prevent copying
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    bool setupDatabase();
    void initializeRepositories();
    void emitDataChanged();

    // Repository instances
    std::unique_ptr<GoalRepository> m_goalRepository;
    std::unique_ptr<TodoRepository> m_todoRepository;
    std::unique_ptr<TimerRepository> m_timerRepository;

    bool m_isInitialized;
};

#endif // DATABASEMANAGER_H