#include "database/repositories/todorepository.h"
#include "database/databaseconstants.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

TodoRepository::TodoRepository(QSqlDatabase& db)
    : m_database(db)
{
}

bool TodoRepository::add(const DatabaseManager::TodoItem& todo)
{
    QSqlQuery query(m_database);
    query.prepare("INSERT INTO todos (goal_id, title, description, is_completed, priority, color_code, "
                  "start_date, end_date) VALUES (?, ?, ?, ?, ?, ?, ?, ?)");
    
    if (!bindTodoToQuery(query, todo)) {
        return false;
    }
    
    if (!query.exec()) {
        qDebug() << "Failed to add todo:" << query.lastError().text();
        return false;
    }
    
    return true;
}

bool TodoRepository::update(const DatabaseManager::TodoItem& todo)
{
    QSqlQuery query(m_database);
    query.prepare("UPDATE todos SET goal_id = ?, title = ?, description = ?, "
                 "is_completed = ?, priority = ?, color_code = ?, start_date = ?, end_date = ? WHERE id = ?");

    query.addBindValue(todo.id);
    
    if (!query.exec()) {
        qDebug() << "Failed to update todo:" << query.lastError().text();
        return false;
    }
    
    return true;
}

bool TodoRepository::remove(int id)
{
    QSqlQuery query(m_database);
    query.prepare("DELETE FROM todos WHERE id = ?");
    query.addBindValue(id);
    
    if (!query.exec()) {
        qDebug() << "Failed to delete todo:" << query.lastError().text();
        return false;
    }
    
    return true;
}

std::optional<DatabaseManager::TodoItem> TodoRepository::findById(int id) const
{
    QSqlQuery query(m_database);
    query.prepare(DatabaseQueries::SELECT_TODO_BY_ID);
    query.addBindValue(id);
    
    if (!query.exec()) {
        qDebug() << "Failed to find todo by id:" << query.lastError().text();
        return std::nullopt;
    }
    
    if (query.next()) {
        return mapFromQuery(query);
    }
    
    return std::nullopt;
}

QList<DatabaseManager::TodoItem> TodoRepository::findAll(bool includeCompleted) const
{
    QList<DatabaseManager::TodoItem> todos;
    QSqlQuery query(m_database);
    
    QString queryStr = DatabaseQueries::SELECT_ALL_TODOS;
    if (!includeCompleted) {
        queryStr = "SELECT * FROM todos WHERE is_completed = 0 ORDER BY priority DESC, id ASC";
    }
    
    if (!query.exec(queryStr)) {
        qDebug() << "Failed to get all todos:" << query.lastError().text();
        return todos;
    }
    
    while (query.next()) {
        todos.append(mapFromQuery(query));
    }
    
    return todos;
}

QList<DatabaseManager::TodoItem> TodoRepository::findByDateRange(const QDate& startDate, const QDate& endDate, bool includeCompleted) const
{
    QList<DatabaseManager::TodoItem> todos;
    QSqlQuery query(m_database);
    
    QString sql = "SELECT id, title, description, priority, is_completed, goal_id, color_code, start_date, end_date, last_update "
                  "FROM todos WHERE ((start_date BETWEEN ? AND ?) OR (end_date BETWEEN ? AND ?) OR "
                  "(start_date <= ? AND end_date >= ?))";
    if (!includeCompleted) {
        sql += " AND is_completed = 0";
    }
    sql += " ORDER BY start_date ASC";
    
    query.prepare(sql);
    query.addBindValue(startDate);
    query.addBindValue(endDate);
    query.addBindValue(startDate);
    query.addBindValue(endDate);
    query.addBindValue(startDate);
    query.addBindValue(endDate);
    
    if (!query.exec()) {
        qDebug() << "Failed to get todos by date range:" << query.lastError().text();
        return todos;
    }
    
    while (query.next()) {
        todos.append(mapFromQuery(query));
    }
    
    return todos;
}

bool TodoRepository::toggleCompletion(int id, bool completed)
{
    QSqlQuery query(m_database);
    query.prepare("UPDATE todos SET is_completed = ? WHERE id = ?");
    query.addBindValue(completed);
    query.addBindValue(id);
    
    if (!query.exec()) {
        qDebug() << "Failed to toggle todo completion:" << query.lastError().text();
        return false;
    }
    
    return true;
}

bool TodoRepository::clear()
{
    QSqlQuery query(m_database);
    if (!query.exec("DELETE FROM todos")) {
        qDebug() << "Failed to clear todos:" << query.lastError().text();
        return false;
    }
    
    return true;
}

DatabaseManager::TodoItem TodoRepository::mapFromQuery(const QSqlQuery& query) const
{
    DatabaseManager::TodoItem todo;
    todo.id = query.value("id").toInt();
    todo.title = query.value("title").toString();
    todo.description = query.value("description").toString();
    todo.priority = static_cast<DatabaseManager::TodoPriority>(query.value("priority").toInt());
    todo.isCompleted = query.value("is_completed").toBool();
    todo.goalId = query.value("goal_id").toInt();
    todo.colorCode = query.value("color_code").toString();
    todo.startDate = query.value("start_date").toDate();
    todo.endDate = query.value("end_date").toDate();
    todo.lastUpdate = query.value("last_update").toDateTime();
    
    return todo;
}

bool TodoRepository::bindTodoToQuery(QSqlQuery& query, const DatabaseManager::TodoItem& todo) const
{
    query.addBindValue(todo.goalId);
    query.addBindValue(todo.title);
    query.addBindValue(todo.description);
    query.addBindValue(todo.isCompleted);
    query.addBindValue(static_cast<int>(todo.priority));
    query.addBindValue(todo.colorCode);
    query.addBindValue(todo.startDate);
    query.addBindValue(todo.endDate);
    
    return true;
}