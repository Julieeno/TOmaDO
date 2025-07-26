#ifndef TODOREPOSITORY_H
#define TODOREPOSITORY_H

#include "database/databasemanager.h"
#include <QList>
#include <QSqlDatabase>
#include <QDate>
#include <optional>

class TodoRepository {
public:
    explicit TodoRepository(QSqlDatabase& db);
    
    bool add(const DatabaseManager::TodoItem& todo);
    bool update(const DatabaseManager::TodoItem& todo);
    bool remove(int id);
    
    std::optional<DatabaseManager::TodoItem> findById(int id) const;
    QList<DatabaseManager::TodoItem> findAll(bool includeCompleted = true) const;
    QList<DatabaseManager::TodoItem> findByDateRange(const QDate& startDate, const QDate& endDate, bool includeCompleted = true) const;
    
    bool toggleCompletion(int id, bool completed);
    bool clear();
    
private:
    DatabaseManager::TodoItem mapFromQuery(const QSqlQuery& query) const;
    bool bindTodoToQuery(QSqlQuery& query, const DatabaseManager::TodoItem& todo) const;
    
    QSqlDatabase& m_database;
};

#endif // TODOREPOSITORY_H