#ifndef GOALREPOSITORY_H
#define GOALREPOSITORY_H

#include "database/databasemanager.h"
#include <QList>
#include <QSqlDatabase>
#include <optional>

class GoalRepository {
public:
    explicit GoalRepository(QSqlDatabase& db);
    
    bool add(const DatabaseManager::GoalItem& goal);
    bool update(const DatabaseManager::GoalItem& goal);
    bool remove(int id);
    
    std::optional<DatabaseManager::GoalItem> findById(int id) const;
    QList<DatabaseManager::GoalItem> findAll(bool includeCompleted = true) const;
    
    bool toggleCompletion(int id, bool completed);
    bool markAsCompleted(int id);
    
    int getTimeSpent(int goalId) const;
    int getTimeSpentInPeriod(int goalId, const QDate& startDate, const QDate& endDate) const;
    
private:
    DatabaseManager::GoalItem mapFromQuery(const QSqlQuery& query) const;
    bool bindGoalToQuery(QSqlQuery& query, const DatabaseManager::GoalItem& goal) const;
    
    QSqlDatabase& m_database;
};

#endif // GOALREPOSITORY_H