#ifndef GOALDIALOGCONSTANTS_H
#define GOALDIALOGCONSTANTS_H

#include <QString>
#include <QMap>
#include "database/databasemanager.h"

namespace GoalDialogConstants {
    // Dialog dimensions
    constexpr int DIALOG_WIDTH = 500;
    constexpr int DIALOG_HEIGHT = 650;
    
    // Layout spacing
    constexpr int MAIN_LAYOUT_SPACING = 20;
    constexpr int FORM_LAYOUT_SPACING = 15;
    constexpr int GROUP_LAYOUT_SPACING = 15;
    constexpr int MAIN_LAYOUT_MARGIN = 20;
    
    // Field constraints
    constexpr int DESCRIPTION_MAX_HEIGHT = 100;
    constexpr int TITLE_MAX_LENGTH = 255;
    constexpr int DESCRIPTION_MAX_LENGTH = 1000;
    constexpr int TIME_INPUT_MAX_VALUE = 999999; // Maximum minutes
    
    // Default values
    const QString DEFAULT_COLOR = "#FF5733";
    constexpr int DEFAULT_ESTIMATED_MINUTES = 60;
    const DatabaseManager::TodoPriority DEFAULT_PRIORITY = DatabaseManager::TodoPriority::Medium;
    
    // Time conversion
    constexpr int SECONDS_PER_MINUTE = 60;
    constexpr int MINUTES_PER_HOUR = 60;
    constexpr int SECONDS_PER_HOUR = SECONDS_PER_MINUTE * MINUTES_PER_HOUR;
    
    // Validation messages
    const QString TITLE_REQUIRED_MESSAGE = QObject::tr("Please enter a goal title.");
    const QString INVALID_INPUT_TITLE = QObject::tr("Invalid Input");
    const QString INVALID_TIME_MESSAGE = QObject::tr("Please enter a valid estimated time.");
}

namespace GoalDialogStrings {
    // Dialog titles
    const QString ADD_GOAL_TITLE = QObject::tr("Add Goal");
    const QString EDIT_GOAL_TITLE = QObject::tr("Edit Goal");
    
    // Form labels
    const QString TITLE_LABEL = QObject::tr("Title:");
    const QString DESCRIPTION_LABEL = QObject::tr("Description:");
    const QString PRIORITY_LABEL = QObject::tr("Priority:");
    const QString ESTIMATED_TIME_LABEL = QObject::tr("Estimated Time (minutes):");
    const QString COMPLETION_LABEL = QObject::tr("Mark as completed");
    
    // Group labels
    const QString COLOR_GROUP_LABEL = QObject::tr("Goal Color");
    const QString DETAILS_GROUP_LABEL = QObject::tr("Goal Details");
    
    // Placeholders
    const QString TITLE_PLACEHOLDER = QObject::tr("Enter goal title...");
    const QString DESCRIPTION_PLACEHOLDER = QObject::tr("Enter goal description...");
    const QString TIME_PLACEHOLDER = QObject::tr("Enter estimated time in minutes...");
    
    // Priority options
    const QMap<DatabaseManager::TodoPriority, QString> PRIORITY_NAMES = {
        {DatabaseManager::TodoPriority::Low, QObject::tr("Low")},
        {DatabaseManager::TodoPriority::Medium, QObject::tr("Medium")},
        {DatabaseManager::TodoPriority::High, QObject::tr("High")}
    };
}

namespace GoalDialogStyles {
    const QString FORM_GROUP_STYLE = 
        "QGroupBox {"
        "   font-weight: 600;"
        "   font-size: 14px;"
        "   color: #2c3e50;"
        "   border: 2px solid #bdc3c7;"
        "   border-radius: 8px;"
        "   margin-top: 10px;"
        "   padding-top: 10px;"
        "}"
        "QGroupBox::title {"
        "   subcontrol-origin: margin;"
        "   left: 10px;"
        "   padding: 0 8px 0 8px;"
        "   background: rgba(255, 255, 255, 200);"
        "   border-radius: 4px;"
        "}";
    
    const QString PRIORITY_COMBOBOX_STYLE = 
        "QComboBox {"
        "   border: 2px solid #bdc3c7;"
        "   border-radius: 6px;"
        "   padding: 8px 12px;"
        "   background: white;"
        "   font-size: 14px;"
        "   min-height: 20px;"
        "}"
        "QComboBox:focus {"
        "   border: 2px solid #3498db;"
        "}"
        "QComboBox::drop-down {"
        "   border: none;"
        "   width: 20px;"
        "}"
        "QComboBox::down-arrow {"
        "   image: none;"
        "   border: 5px solid transparent;"
        "   border-top: 6px solid #7f8c8d;"
        "   margin-right: 5px;"
        "}";
    
    const QString TIME_INPUT_STYLE = 
        "QSpinBox {"
        "   border: 2px solid #bdc3c7;"
        "   border-radius: 6px;"
        "   padding: 8px 12px;"
        "   background: white;"
        "   font-size: 14px;"
        "   min-height: 20px;"
        "}"
        "QSpinBox:focus {"
        "   border: 2px solid #3498db;"
        "}"
        "QSpinBox::up-button, QSpinBox::down-button {"
        "   border: none;"
        "   width: 20px;"
        "   background: transparent;"
        "}"
        "QSpinBox::up-arrow {"
        "   border: 4px solid transparent;"
        "   border-bottom: 6px solid #7f8c8d;"
        "}"
        "QSpinBox::down-arrow {"
        "   border: 4px solid transparent;"
        "   border-top: 6px solid #7f8c8d;"
        "}";
    
    const QString COMPLETION_CHECKBOX_STYLE = 
        "QCheckBox {"
        "   font-size: 14px;"
        "   color: #2c3e50;"
        "   spacing: 8px;"
        "}"
        "QCheckBox::indicator {"
        "   width: 18px;"
        "   height: 18px;"
        "   border: 2px solid #bdc3c7;"
        "   border-radius: 4px;"
        "   background: white;"
        "}"
        "QCheckBox::indicator:checked {"
        "   background: #27ae60;"
        "   border: 2px solid #27ae60;"
        "   image: url(:/icons/checkmark.png);"
        "}"
        "QCheckBox::indicator:hover {"
        "   border: 2px solid #3498db;"
        "}";
}

#endif // GOALDIALOGCONSTANTS_H