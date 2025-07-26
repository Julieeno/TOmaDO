#ifndef DATABASECONNECTIONMANAGER_H
#define DATABASECONNECTIONMANAGER_H

#include <QSqlDatabase>
#include <QString>

class DatabaseConnectionManager {
public:
    static DatabaseConnectionManager& instance();
    
    bool initialize();
    bool isInitialized() const { return m_isInitialized; }
    QSqlDatabase& database() { return m_database; }
    
    bool beginTransaction();
    bool commitTransaction();
    bool rollbackTransaction();
    
    void close();
    
private:
    DatabaseConnectionManager();
    ~DatabaseConnectionManager();
    
    // Prevent copying
    DatabaseConnectionManager(const DatabaseConnectionManager&) = delete;
    DatabaseConnectionManager& operator=(const DatabaseConnectionManager&) = delete;
    
    bool setupDatabase();
    bool createDatabaseDirectory();
    QString getDatabasePath() const;
    
    QSqlDatabase m_database;
    bool m_isInitialized;
};

#endif // DATABASECONNECTIONMANAGER_H