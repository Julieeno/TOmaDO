#ifndef DATABASESCHEMAMANAGER_H
#define DATABASESCHEMAMANAGER_H

#include <QSqlDatabase>

class DatabaseSchemaManager {
public:
    static bool createTables(QSqlDatabase& db);
    static bool createTriggers(QSqlDatabase& db);
    static bool verifySchema(QSqlDatabase& db);
    
private:
    static bool createGoalsTable(QSqlDatabase& db);
    static bool createTodosTable(QSqlDatabase& db);
    static bool createTimerRecordsTable(QSqlDatabase& db);
    static bool createTodosUpdateTrigger(QSqlDatabase& db);
    
    static bool executeQuery(QSqlDatabase& db, const QString& query, const QString& operation);
    
    DatabaseSchemaManager() = delete; // Static utility class
};

#endif // DATABASESCHEMAMANAGER_H