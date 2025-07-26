#include "database/repositories/goalrepository.h"
#include "database/databaseconstants.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

GoalRepository::GoalRepository(QSqlDatabase& db)
    : m_database(db)
{
}

bool GoalRepository::add(const DatabaseManager::GoalItem& goal)
{
    QSqlQuery query(m_database);
    query.prepare("INSERT INTO goals (title, description, is_completed, priority, color_code, estimated_time) "
                 "VALUES (?, ?, ?, ?, ?, ?)");
    
    if (!bindGoalToQuery(query, goal)) {
        return false;
    }
    
    if (!query.exec()) {
        qDebug() << "Failed to add goal:" << query.lastError().text();
        return false;
    }
    
    return true;
}

bool GoalRepository::update(const DatabaseManager::GoalItem& goal)
{
    QSqlQuery query(m_database);
    query.prepare("UPDATE goals SET title = ?, description = ?, is_completed = ?, "
                 "priority = ?, color_code = ?, estimated_time = ? WHERE id = ?");
    
    if (!bindGoalToQuery(query, goal)) {
        return false;
    }
    query.addBindValue(goal.id);
    
    if (!query.exec()) {
        qDebug() << "Failed to update goal:" << query.lastError().text();
        return false;
    }
    
    return true;
}

bool GoalRepository::remove(int id)
{
    QSqlQuery query(m_database);
    query.prepare("DELETE FROM goals WHERE id = ?");
    query.addBindValue(id);
    
    if (!query.exec()) {
        qDebug() << "Failed to delete goal:" << query.lastError().text();
        return false;
    }
    
    return true;
}

std::optional<DatabaseManager::GoalItem> GoalRepository::findById(int id) const
{
    QSqlQuery query(m_database);
    query.prepare(DatabaseQueries::SELECT_GOAL_BY_ID);
    query.addBindValue(id);
    
    if (!query.exec()) {
        qDebug() << "Failed to find goal by id:" << query.lastError().text();
        return std::nullopt;
    }
    
    if (query.next()) {
        return mapFromQuery(query);
    }
    
    return std::nullopt;
}

QList<DatabaseManager::GoalItem> GoalRepository::findAll(bool includeCompleted) const
{
    QList<DatabaseManager::GoalItem> goals;
    QSqlQuery query(m_database);
    
    QString queryStr = DatabaseQueries::SELECT_ALL_GOALS;
    if (!includeCompleted) {
        queryStr = "SELECT * FROM goals WHERE is_completed = 0 ORDER BY priority DESC, id ASC";
    }
    
    if (!query.exec(queryStr)) {
        qDebug() << "Failed to get all goals:" << query.lastError().text();
        return goals;
    }
    
    while (query.next()) {
        goals.append(mapFromQuery(query));
    }
    
    return goals;
}

bool GoalRepository::toggleCompletion(int id, bool completed)
{
    QSqlQuery query(m_database);
    query.prepare("UPDATE goals SET is_completed = ? WHERE id = ?");
    query.addBindValue(completed);
    query.addBindValue(id);
    
    if (!query.exec()) {
        qDebug() << "Failed to toggle goal completion:" << query.lastError().text();
        return false;
    }
    
    return true;
}

bool GoalRepository::markAsCompleted(int id)
{
    return toggleCompletion(id, true);
}

int GoalRepository::getTimeSpent(int goalId) const
{
    QSqlQuery query(m_database);
    query.prepare("SELECT SUM(strftime('%s', end_time) - strftime('%s', start_time)) "
                 "FROM timer_records WHERE goal_id = ?");
    query.addBindValue(goalId);
    
    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    }
    
    return 0;
}

int GoalRepository::getTimeSpentInPeriod(int goalId, const QDate& startDate, const QDate& endDate) const
{
    QSqlQuery query(m_database);
    query.prepare("SELECT SUM(CAST((julianday(end_time) - julianday(start_time)) * 86400 AS INTEGER)) "
                 "FROM timer_records "
                 "WHERE goal_id = ? AND DATE(start_time) >= ? AND DATE(start_time) <= ?");
    
    query.addBindValue(goalId);
    query.addBindValue(startDate.toString(DatabaseConstants::DATE_FORMAT));
    query.addBindValue(endDate.toString(DatabaseConstants::DATE_FORMAT));
    
    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    }
    
    return 0;
}

DatabaseManager::GoalItem GoalRepository::mapFromQuery(const QSqlQuery& query) const
{
    DatabaseManager::GoalItem goal;
    goal.id = query.value("id").toInt();
    goal.title = query.value("title").toString();
    goal.description = query.value("description").toString();
    goal.isCompleted = query.value("is_completed").toBool();
    goal.priority = static_cast<DatabaseManager::TodoPriority>(query.value("priority").toInt());
    goal.colorCode = query.value("color_code").toString();
    goal.estimatedTime = query.value("estimated_time").toInt();
    
    return goal;
}

bool GoalRepository::bindGoalToQuery(QSqlQuery& query, const DatabaseManager::GoalItem& goal) const
{
    query.addBindValue(goal.title);
    query.addBindValue(goal.description);
    query.addBindValue(goal.isCompleted);
    query.addBindValue(static_cast<int>(goal.priority));
    query.addBindValue(goal.colorCode);
    query.addBindValue(goal.estimatedTime);
    
    return true;
}