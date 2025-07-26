#ifndef TIMERREPOSITORY_H
#define TIMERREPOSITORY_H

#include "database/databasemanager.h"
#include <QList>
#include <QSqlDatabase>
#include <QDateTime>
#include <optional>

class TimerRepository {
public:
    explicit TimerRepository(QSqlDatabase& db);
    
    bool add(const DatabaseManager::TimerRecord& record);
    std::optional<DatabaseManager::TimerRecord> findById(int id) const;
    QList<DatabaseManager::TimerRecord> findByDateRange(const QDateTime& start, const QDateTime& end) const;
    
    bool clear();
    
private:
    DatabaseManager::TimerRecord mapFromQuery(const QSqlQuery& query) const;
    
    QSqlDatabase& m_database;
};

#endif // TIMERREPOSITORY_H