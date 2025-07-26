#ifndef DATABASECONSTANTS_H
#define DATABASECONSTANTS_H

#include <QString>
#include <QMap>

namespace DatabaseConstants {
    // Database configuration
    const QString DB_DRIVER = "QSQLITE";
    const QString DB_RELATIVE_PATH = "/../db/tomado.db";
    const QString DB_DIRECTORY = "/../db";
    const QString DB_NAME = "tomado.db";
    
    // Table names
    const QString TABLE_GOALS = "goals";
    const QString TABLE_TODOS = "todos";
    const QString TABLE_TIMER_RECORDS = "timer_records";
    
    // Default values
    const int DEFAULT_PRIORITY = 1;
    const int DEFAULT_ESTIMATED_TIME = 0;
    const bool DEFAULT_COMPLETED = false;
    
    // Query limits and performance
    const int MAX_QUERY_RESULTS = 1000;
    const int BATCH_SIZE = 100;
    
    // Date format
    const QString DATE_FORMAT = "yyyy-MM-dd";
    
    // Time calculation constants
    const int SECONDS_PER_DAY = 86400;
}

namespace DatabaseQueries {
    // Table creation queries
    const QString CREATE_GOALS_TABLE = 
        "CREATE TABLE IF NOT EXISTS goals ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "title TEXT NOT NULL,"
        "description TEXT,"
        "is_completed BOOLEAN DEFAULT 0,"
        "priority INTEGER DEFAULT 0,"
        "color_code TEXT,"
        "estimated_time INTEGER DEFAULT 0"
        ")";
    
    const QString CREATE_TODOS_TABLE = 
        "CREATE TABLE IF NOT EXISTS todos ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "title TEXT NOT NULL, "
        "description TEXT, "
        "priority INTEGER DEFAULT 1, "
        "is_completed BOOLEAN DEFAULT 0, "
        "goal_id INTEGER, "
        "color_code TEXT, "
        "start_date DATE, "
        "end_date DATE, "
        "last_update DATETIME DEFAULT CURRENT_TIMESTAMP, "
        "FOREIGN KEY(goal_id) REFERENCES goals(id) ON DELETE CASCADE)";
    
    const QString CREATE_TIMER_RECORDS_TABLE = 
        "CREATE TABLE IF NOT EXISTS timer_records ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "goal_id INTEGER,"
        "start_time DATETIME NOT NULL,"
        "end_time DATETIME NOT NULL,"
        "FOREIGN KEY(goal_id) REFERENCES goals(id)"
        ")";
    
    const QString CREATE_TODOS_UPDATE_TRIGGER = 
        "CREATE TRIGGER IF NOT EXISTS update_todos_last_update "
        "AFTER UPDATE ON todos "
        "FOR EACH ROW "
        "BEGIN "
        "    UPDATE todos SET last_update = CURRENT_TIMESTAMP WHERE id = NEW.id; "
        "END";
    
    // Common queries
    const QString SELECT_ALL_GOALS = 
        "SELECT * FROM goals ORDER BY priority DESC, id ASC";
    
    const QString SELECT_ALL_TODOS = 
        "SELECT * FROM todos ORDER BY priority DESC, id ASC";
    
    const QString SELECT_GOAL_BY_ID = 
        "SELECT id, title, description, priority, is_completed, estimated_time, color_code FROM goals WHERE id = ?";
    
    const QString SELECT_TODO_BY_ID = 
        "SELECT id, title, description, priority, is_completed, goal_id, color_code, start_date, end_date, last_update FROM todos WHERE id = ?";
}

namespace ColorConstants {
    const QStringList AVAILABLE_COLORS = {
        "#FF6B6B", "#4ECDC4", "#45B7D1", "#96CEB4", "#FFEAA7",
        "#DDA0DD", "#98D8C8", "#F7DC6F", "#BB8FCE", "#85C1E9",
        "#F8C471", "#82E0AA", "#F1948A", "#AED6F1", "#D2B4DE"
    };
    
    const QMap<QString, QString> COLOR_NAMES = {
        {"#FF0000", "Red"},
        {"#00FF00", "Green"},
        {"#0000FF", "Blue"},
        {"#FF6B6B", "Coral"},
        {"#4ECDC4", "Turquoise"},
        {"#45B7D1", "Sky Blue"},
        {"#96CEB4", "Mint"},
        {"#FFEAA7", "Yellow"},
        {"#DDA0DD", "Plum"},
        {"#98D8C8", "Seafoam"},
        {"#F7DC6F", "Gold"},
        {"#BB8FCE", "Lavender"},
        {"#85C1E9", "Light Blue"},
        {"#F8C471", "Peach"},
        {"#82E0AA", "Light Green"},
        {"#F1948A", "Salmon"},
        {"#AED6F1", "Powder Blue"},
        {"#D2B4DE", "Mauve"}
    };
}

#endif // DATABASECONSTANTS_H