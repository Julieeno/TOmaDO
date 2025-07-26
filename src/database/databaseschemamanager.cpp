#include "database/databaseschemamanager.h"
#include "database/databaseconstants.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

bool DatabaseSchemaManager::createTables(QSqlDatabase& db)
{
    return createGoalsTable(db) && 
           createTodosTable(db) && 
           createTimerRecordsTable(db);
}

bool DatabaseSchemaManager::createTriggers(QSqlDatabase& db)
{
    return createTodosUpdateTrigger(db);
}

bool DatabaseSchemaManager::verifySchema(QSqlDatabase& db)
{
    QSqlQuery query(db);
    
    // Check if all required tables exist
    query.exec("SELECT name FROM sqlite_master WHERE type='table'");
    QStringList existingTables;
    
    while (query.next()) {
        existingTables << query.value(0).toString();
    }
    
    return existingTables.contains(DatabaseConstants::TABLE_GOALS) &&
           existingTables.contains(DatabaseConstants::TABLE_TODOS) &&
           existingTables.contains(DatabaseConstants::TABLE_TIMER_RECORDS);
}

bool DatabaseSchemaManager::createGoalsTable(QSqlDatabase& db)
{
    return executeQuery(db, DatabaseQueries::CREATE_GOALS_TABLE, "create goals table");
}

bool DatabaseSchemaManager::createTodosTable(QSqlDatabase& db)
{
    return executeQuery(db, DatabaseQueries::CREATE_TODOS_TABLE, "create todos table");
}

bool DatabaseSchemaManager::createTimerRecordsTable(QSqlDatabase& db)
{
    return executeQuery(db, DatabaseQueries::CREATE_TIMER_RECORDS_TABLE, "create timer_records table");
}

bool DatabaseSchemaManager::createTodosUpdateTrigger(QSqlDatabase& db)
{
    return executeQuery(db, DatabaseQueries::CREATE_TODOS_UPDATE_TRIGGER, "create todos update trigger");
}

bool DatabaseSchemaManager::executeQuery(QSqlDatabase& db, const QString& query, const QString& operation)
{
    QSqlQuery sqlQuery(db);
    if (!sqlQuery.exec(query)) {
        qDebug() << "Failed to" << operation << ":" << sqlQuery.lastError().text();
        return false;
    }
    return true;
}