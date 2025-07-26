#include "database/databaseconnectionmanager.h"
#include "database/databaseconstants.h"
#include <QCoreApplication>
#include <QDir>
#include <QSqlError>
#include <QDebug>

#ifdef NDEBUG
    const QString DB_PATH = QString("%1/share/tomado/db/tomado.db").arg(QCoreApplication::applicationDirPath());
#else
const QString DB_PATH = "db/tomado.db";
#endif

DatabaseConnectionManager& DatabaseConnectionManager::instance()
{
    static DatabaseConnectionManager instance;
    return instance;
}

DatabaseConnectionManager::DatabaseConnectionManager()
    : m_database(QSqlDatabase::addDatabase(DatabaseConstants::DB_DRIVER))
    , m_isInitialized(false)
{
}

DatabaseConnectionManager::~DatabaseConnectionManager()
{
    close();
}

bool DatabaseConnectionManager::initialize()
{
    if (m_isInitialized) {
        return true;
    }

    if (!setupDatabase()) {
        qDebug() << "Failed to setup database";
        return false;
    }
    
    m_isInitialized = true;
    return true;
}

bool DatabaseConnectionManager::setupDatabase()
{
#ifdef NDEBUG
    // Release mode - use installed database path
    QString dbDir = QDir::homePath() + "/.local/share/tomado/db";
    QDir dir(dbDir);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    QString dbFilePath = dbDir + "/tomado.db";
#else
    // Debug mode - use local project database path
    QString dbFilePath = DB_PATH;
    QDir dir(QFileInfo(dbFilePath).absolutePath());
    if (!dir.exists()) {
        dir.mkpath(".");
    }
#endif

    m_database.setDatabaseName(dbFilePath);
    
    if (!m_database.open()) {
        qDebug() << "Failed to open database:" << m_database.lastError().text();
        return false;
    }
    
    return true;
}

bool DatabaseConnectionManager::createDatabaseDirectory()
{
    QString appDir = QCoreApplication::applicationDirPath();
    QString dbDirPath = appDir + DatabaseConstants::DB_DIRECTORY;
    
    QDir dbDir(dbDirPath);
    if (!dbDir.exists() && !dbDir.mkpath(".")) {
        qDebug() << "Failed to create database directory:" << dbDirPath;
        return false;
    }
    
    return true;
}

QString DatabaseConnectionManager::getDatabasePath() const
{
#ifdef NDEBUG
    return QDir::homePath() + "/.local/share/tomado/db/tomado.db";
#else
    return DB_PATH;
#endif
}

bool DatabaseConnectionManager::beginTransaction()
{
    if (!m_isInitialized) {
        return false;
    }
    return m_database.transaction();
}

bool DatabaseConnectionManager::commitTransaction()
{
    if (!m_isInitialized) {
        return false;
    }
    return m_database.commit();
}

bool DatabaseConnectionManager::rollbackTransaction()
{
    if (!m_isInitialized) {
        return false;
    }
    return m_database.rollback();
}

void DatabaseConnectionManager::close()
{
    if (m_database.isOpen()) {
        m_database.close();
    }
    m_isInitialized = false;
}