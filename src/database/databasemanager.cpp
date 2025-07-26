
#include "database/databasemanager.h"
#include "database/databaseconnectionmanager.h"
#include "database/databaseschemamanager.h"
#include "database/repositories/goalrepository.h"
#include "database/repositories/todorepository.h"
#include "database/repositories/timerrepository.h"
#include "database/services/colorservice.h"
#include <QDebug>

DatabaseManager& DatabaseManager::instance()
{
    static DatabaseManager instance;
    return instance;
}

DatabaseManager::DatabaseManager()
    : QObject()
    , m_isInitialized(false)
{
}

DatabaseManager::~DatabaseManager()
{
    // Repository cleanup handled by smart pointers
}

bool DatabaseManager::initialize()
{
    if (m_isInitialized) {
        return true;
    }

    if (!setupDatabase()) {
        qDebug() << "Failed to setup database";
        return false;
    }

    initializeRepositories();
    m_isInitialized = true;
    return true;
}

bool DatabaseManager::isInitialized() const
{
    return m_isInitialized;
}

bool DatabaseManager::setupDatabase()
{
    auto& connectionManager = DatabaseConnectionManager::instance();

    if (!connectionManager.initialize()) {
        return false;
    }

    QSqlDatabase& db = connectionManager.database();

    if (!DatabaseSchemaManager::createTables(db)) {
        return false;
    }

    if (!DatabaseSchemaManager::createTriggers(db)) {
        return false;
    }

    return DatabaseSchemaManager::verifySchema(db);
}

void DatabaseManager::initializeRepositories()
{
    QSqlDatabase& db = DatabaseConnectionManager::instance().database();

    m_goalRepository = std::make_unique<GoalRepository>(db);
    m_todoRepository = std::make_unique<TodoRepository>(db);
    m_timerRepository = std::make_unique<TimerRepository>(db);
}

void DatabaseManager::emitDataChanged()
{
    emit dataChanged();
}

// Timer records operations
bool DatabaseManager::addTimerRecord(const TimerRecord& record) {
    if (!m_timerRepository) {
        qDebug() << "Timer repository not initialized!";
        return false;
    }

    bool success = m_timerRepository->add(record);

    if (success) {
        emitDataChanged();
    }

    return success;
}

DatabaseManager::TimerRecord DatabaseManager::getTimerRecord(int id)
{
    if (!m_timerRepository) {
        return TimerRecord();
    }

    auto record = m_timerRepository->findById(id);
    return record.value_or(TimerRecord());
}

QList<DatabaseManager::TimerRecord> DatabaseManager::getTimerRecords(const QDateTime& start, const QDateTime& end)
{
    if (!m_timerRepository) {
        return QList<TimerRecord>();
    }

    return m_timerRepository->findByDateRange(start, end);
}

bool DatabaseManager::clearTimerRecords()
{
    if (!m_timerRepository) {
        return false;
    }

    auto& connectionManager = DatabaseConnectionManager::instance();

    if (!connectionManager.beginTransaction()) {
        return false;
    }

    bool success = m_timerRepository->clear();

    if (success) {
        connectionManager.commitTransaction();
        emitDataChanged();
    } else {
        connectionManager.rollbackTransaction();
    }

    return success;
}

// Goals operations
bool DatabaseManager::addGoal(const GoalItem& goal)
{
    if (!m_goalRepository) {
        return false;
    }

    bool result = m_goalRepository->add(goal);
    if (result) {
        emitDataChanged();
    }
    return result;
}

bool DatabaseManager::updateGoal(const GoalItem& goal)
{
    if (!m_goalRepository) {
        return false;
    }

    bool result = m_goalRepository->update(goal);
    if (result) {
        emitDataChanged();
    }
    return result;
}

bool DatabaseManager::deleteGoal(int id)
{
    if (!m_goalRepository) {
        return false;
    }

    bool result = m_goalRepository->remove(id);
    if (result) {
        emitDataChanged();
    }
    return result;
}

DatabaseManager::GoalItem DatabaseManager::getGoal(int id)
{
    if (!m_goalRepository) {
        return GoalItem();
    }

    auto goal = m_goalRepository->findById(id);
    return goal.value_or(GoalItem());
}

QList<DatabaseManager::GoalItem> DatabaseManager::getAllGoals(bool includeCompleted)
{
    if (!m_goalRepository) {
        return QList<GoalItem>();
    }

    return m_goalRepository->findAll(includeCompleted);
}

bool DatabaseManager::toggleGoalCompletion(int id, bool completed)
{
    if (!m_goalRepository) {
        return false;
    }

    bool result = m_goalRepository->toggleCompletion(id, completed);
    if (result) {
        emitDataChanged();
    }
    return result;
}

bool DatabaseManager::completeGoal(int goalId)
{
    if (!m_goalRepository) {
        return false;
    }

    bool result = m_goalRepository->markAsCompleted(goalId);
    if (result) {
        emitDataChanged();
    }
    return result;
}

int DatabaseManager::getGoalTimeSpent(int goalId)
{
    if (!m_goalRepository) {
        return 0;
    }

    return m_goalRepository->getTimeSpent(goalId);
}

int DatabaseManager::getGoalTimeSpentInPeriod(int goalId, const QDate& startDate, const QDate& endDate)
{
    if (!m_goalRepository) {
        return 0;
    }

    return m_goalRepository->getTimeSpentInPeriod(goalId, startDate, endDate);
}

// Todos operations
bool DatabaseManager::addTodo(const TodoItem& todo)
{
    if (!m_todoRepository) {
        return false;
    }

    bool result = m_todoRepository->add(todo);
    if (result) {
        emitDataChanged();
    }
    return result;
}

bool DatabaseManager::updateTodo(const TodoItem& todo)
{
    if (!m_todoRepository) {
        return false;
    }

    bool result = m_todoRepository->update(todo);
    if (result) {
        emitDataChanged();
    }
    return result;
}

bool DatabaseManager::deleteTodo(int id)
{
    if (!m_todoRepository) {
        return false;
    }

    bool result = m_todoRepository->remove(id);
    if (result) {
        emitDataChanged();
    }
    return result;
}

DatabaseManager::TodoItem DatabaseManager::getTodo(int id)
{
    if (!m_todoRepository) {
        return TodoItem();
    }

    auto todo = m_todoRepository->findById(id);
    return todo.value_or(TodoItem());
}

QList<DatabaseManager::TodoItem> DatabaseManager::getAllTodos(bool includeCompleted)
{
    if (!m_todoRepository) {
        return QList<TodoItem>();
    }

    return m_todoRepository->findAll(includeCompleted);
}

bool DatabaseManager::toggleTodoCompletion(int id, bool completed)
{
    if (!m_todoRepository) {
        return false;
    }

    bool result = m_todoRepository->toggleCompletion(id, completed);
    if (result) {
        emitDataChanged();
    }
    return result;
}

QList<DatabaseManager::TodoItem> DatabaseManager::getTodosByDateRange(const QDate& startDate, const QDate& endDate, bool includeCompleted)
{
    if (!m_todoRepository) {
        return QList<TodoItem>();
    }

    return m_todoRepository->findByDateRange(startDate, endDate, includeCompleted);
}

bool DatabaseManager::clearTodos()
{
    if (!m_todoRepository) {
        return false;
    }

    auto& connectionManager = DatabaseConnectionManager::instance();

    if (!connectionManager.beginTransaction()) {
        return false;
    }

    bool success = m_todoRepository->clear();

    if (success) {
        connectionManager.commitTransaction();
        emitDataChanged();
    } else {
        connectionManager.rollbackTransaction();
    }

    return success;
}

// Colors operations
QString DatabaseManager::getColorName(const QString& colorCode)
{
    return ColorService::instance().getColorName(colorCode);
}

QStringList DatabaseManager::getAvailableColors()
{
    return ColorService::instance().getAvailableColors();
}