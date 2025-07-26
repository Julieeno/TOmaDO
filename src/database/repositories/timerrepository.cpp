#include "database/repositories/timerrepository.h"
#include "database/databaseconstants.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

TimerRepository::TimerRepository(QSqlDatabase& db)
    : m_database(db)
{
}

bool TimerRepository::add(const DatabaseManager::TimerRecord& record)
{
    QSqlQuery query(m_database);
    query.prepare("INSERT INTO timer_records (goal_id, start_time, end_time) VALUES (?, ?, ?)");
    query.addBindValue(record.goalId);
    query.addBindValue(record.startTime);
    query.addBindValue(record.endTime);
    
    if (!query.exec()) {
        qDebug() << "Failed to add timer record:" << query.lastError().text();
        return false;
    }
    
    return true;
}

std::optional<DatabaseManager::TimerRecord> TimerRepository::findById(int id) const
{
    QSqlQuery query(m_database);
    query.prepare("SELECT * FROM timer_records WHERE id = ?");
    query.addBindValue(id);
    
    if (!query.exec()) {
        qDebug() << "Failed to find timer record by id:" << query.lastError().text();
        return std::nullopt;
    }
    
    if (query.next()) {
        return mapFromQuery(query);
    }
    
    return std::nullopt;
}

QList<DatabaseManager::TimerRecord> TimerRepository::findByDateRange(const QDateTime& start, const QDateTime& end) const
{
    QList<DatabaseManager::TimerRecord> records;
    QSqlQuery query(m_database);
    
    query.prepare(
        "SELECT * FROM timer_records WHERE "
        "(start_time BETWEEN ? AND ?) OR "
        "(end_time BETWEEN ? AND ?) OR "
        "(start_time <= ? AND end_time >= ?) "
        "ORDER BY start_time DESC"
    );
    
    query.addBindValue(start);
    query.addBindValue(end);
    query.addBindValue(start);
    query.addBindValue(end);
    query.addBindValue(start);
    query.addBindValue(end);
    
    if (!query.exec()) {
        qDebug() << "Failed to get timer records by date range:" << query.lastError().text();
        return records;
    }
    
    while (query.next()) {
        records.append(mapFromQuery(query));
    }
    
    return records;
}

bool TimerRepository::clear()
{
    QSqlQuery query(m_database);
    if (!query.exec("DELETE FROM timer_records")) {
        qDebug() << "Failed to clear timer records:" << query.lastError().text();
        return false;
    }
    
    return true;
}

DatabaseManager::TimerRecord TimerRepository::mapFromQuery(const QSqlQuery& query) const
{
    DatabaseManager::TimerRecord record;
    record.id = query.value("id").toInt();
    record.goalId = query.value("goal_id").toInt();
    record.startTime = query.value("start_time").toDateTime();
    record.endTime = query.value("end_time").toDateTime();
    
    return record;
}