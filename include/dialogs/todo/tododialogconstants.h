#ifndef TODODIALOGCONSTANTS_H
#define TODODIALOGCONSTANTS_H

#include <QString>
#include <QMap>
#include "database/databasemanager.h"

namespace TodoDialogConstants {
    // Dialog dimensions
    constexpr int DIALOG_WIDTH = 600;
    constexpr int DIALOG_HEIGHT = 700;

    // Layout spacing
    constexpr int MAIN_LAYOUT_SPACING = 15;
    constexpr int FORM_LAYOUT_SPACING = 10;
    constexpr int GROUP_LAYOUT_SPACING = 10;
    constexpr int MAIN_LAYOUT_MARGIN = 20;

    // Field constraints
    constexpr int DESCRIPTION_MAX_HEIGHT = 80;
    constexpr int TITLE_MAX_LENGTH = 255;
    constexpr int DESCRIPTION_MAX_LENGTH = 1000;

    // Default values
    const QString DEFAULT_COLOR = "#FF6B7A";
    const DatabaseManager::TodoPriority DEFAULT_PRIORITY = DatabaseManager::TodoPriority::Medium;

    // Date/time formats
    const QString DATE_TIME_FORMAT = "dd/MM/yyyy hh:mm";
    const QString DATE_FORMAT = "dd/MM/yyyy";

    // Validation messages
    const QString TITLE_REQUIRED_MESSAGE = QObject::tr("Please enter a title for the todo.");
    const QString INVALID_INPUT_TITLE = QObject::tr("Invalid Input");
    const QString DATE_ERROR_TITLE = QObject::tr("Date Error");
    const QString DATE_ERROR_MESSAGE = QObject::tr("Due date must be after start date.\n\nPlease adjust your dates so the due date comes after the start date.");
}

namespace TodoDialogStrings {
    // Dialog titles
    const QString ADD_TODO_TITLE = QObject::tr("Add Todo");
    const QString EDIT_TODO_TITLE = QObject::tr("Edit Todo");

    // Form labels
    const QString TITLE_LABEL = QObject::tr("Title:");
    const QString DESCRIPTION_LABEL = QObject::tr("Description:");
    const QString GOAL_LABEL = QObject::tr("Associated Goal:");
    const QString PRIORITY_LABEL = QObject::tr("Priority:");
    const QString COMPLETION_LABEL = QObject::tr("Mark as completed");
    const QString START_DATE_LABEL = QObject::tr("Start date time:");
    const QString DUE_DATE_LABEL = QObject::tr("Due date time:");

    // Group labels
    const QString SCHEDULE_GROUP_LABEL = QObject::tr("Schedule");
    const QString DETAILS_GROUP_LABEL = QObject::tr("Todo Details");

    // Placeholders
    const QString TITLE_PLACEHOLDER = QObject::tr("Enter todo title...");
    const QString DESCRIPTION_PLACEHOLDER = QObject::tr("Enter todo description...");

    // Other text
    const QString NO_GOAL_OPTION = QObject::tr("No Goal");
    const QString SCHEDULE_NOTE = QObject::tr("💡 Both dates are optional. Use them to schedule your tasks.");
    const QString SAVE_BUTTON = QObject::tr("Save");
    const QString CANCEL_BUTTON = QObject::tr("Cancel");

    // Priority options
    const QMap<DatabaseManager::TodoPriority, QString> PRIORITY_NAMES = {
        {DatabaseManager::TodoPriority::Low, QObject::tr("Low")},
        {DatabaseManager::TodoPriority::Medium, QObject::tr("Medium")},
        {DatabaseManager::TodoPriority::High, QObject::tr("High")}
    };
}

namespace TodoDialogStyles {
    const QString MAIN_DIALOG_STYLE =
        "QDialog {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "       stop:0 #FFFBFB, stop:0.5 #FFF5F5, stop:1 #FFEFEF);"
        "   border-radius: 12px;"
        "}";

    const QString FORM_LABEL_STYLE =
        "QLabel {"
        "   color: #2c3e50;"
        "   font-weight: 600;"
        "   font-size: 14px;"
        "   background: transparent;"
        "}";

    const QString INPUT_FIELD_STYLE =
        "QLineEdit, QTextEdit {"
        "   background: rgba(255, 255, 255, 180);"
        "   border: 2px solid #FFE0E0;"
        "   border-radius: 8px;"
        "   padding: 8px 12px;"
        "   font-size: 14px;"
        "   color: #2c3e50;"
        "}"
        "QLineEdit:focus, QTextEdit:focus {"
        "   border: 2px solid #FF6B7A;"
        "   background: rgba(255, 255, 255, 220);"
        "}";

    const QString COMBOBOX_STYLE =
        "QComboBox {"
        "   background: rgba(255, 255, 255, 180);"
        "   border: 2px solid #FFE0E0;"
        "   border-radius: 8px;"
        "   padding: 8px 12px;"
        "   font-size: 14px;"
        "   color: #2c3e50;"
        "}"
        "QComboBox:focus {"
        "   border: 2px solid #FF6B7A;"
        "   background: rgba(255, 255, 255, 220);"
        "}";

    const QString DATETIME_STYLE =
        "QDateTimeEdit {"
        "   background: rgba(255, 255, 255, 180);"
        "   border: 2px solid #FFE0E0;"
        "   border-radius: 8px;"
        "   padding: 8px 12px;"
        "   font-size: 14px;"
        "   color: #2c3e50;"
        "}"
        "QDateTimeEdit:focus {"
        "   border: 2px solid #FF6B7A;"
        "   background: rgba(255, 255, 255, 220);"
        "}";
    
    const QString CHECKBOX_STYLE = 
        "QCheckBox {"
        "   color: #2c3e50;"
        "   font-weight: 500;"
        "   font-size: 14px;"
        "   spacing: 8px;"
        "}"
        "QCheckBox::indicator {"
        "   width: 18px;"
        "   height: 18px;"
        "   border: 2px solid #FFE0E0;"
        "   border-radius: 4px;"
        "   background: rgba(255, 255, 255, 180);"
        "}"
        "QCheckBox::indicator:checked {"
        "   background: #FF6B7A;"
        "   border: 2px solid #FF6B7A;"
        "}"
        "QCheckBox::indicator:hover {"
        "   border: 2px solid #FF6B7A;"
        "}";
    
    const QString BUTTON_STYLE = 
        "QPushButton {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "       stop:0 #FF6B7A, stop:1 #FF5A6B);"
        "   color: white;"
        "   border: none;"
        "   border-radius: 8px;"
        "   padding: 10px 20px;"
        "   font-weight: 600;"
        "   font-size: 14px;"
        "   min-width: 80px;"
        "}"
        "QPushButton:hover {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "       stop:0 #FF5A6B, stop:1 #FF4D5E);"
        "}"
        "QPushButton#cancelButton {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "       stop:0 #bdc3c7, stop:1 #95a5a6);"
        "}"
        "QPushButton#cancelButton:hover {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "       stop:0 #95a5a6, stop:1 #7f8c8d);"
        "}";
    
    const QString GROUPBOX_STYLE = 
        "QGroupBox {"
        "   font-weight: 600;"
        "   font-size: 16px;"
        "   color: #2c3e50;"
        "   border: 2px solid #FFE0E0;"
        "   border-radius: 12px;"
        "   margin-top: 12px;"
        "   padding-top: 15px;"
        "   background: rgba(255, 255, 255, 80);"
        "}"
        "QGroupBox::title {"
        "   subcontrol-origin: margin;"
        "   left: 15px;"
        "   padding: 5px 12px;"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "       stop:0 #FFE5E5, stop:1 #FFF0F0);"
        "   border-radius: 6px;"
        "   border: 1px solid #FFD6D6;"
        "}";
    
    const QString NOTE_LABEL_STYLE = 
        "QLabel {"
        "   color: #7f8c8d;"
        "   font-size: 11px;"
        "   font-style: italic;"
        "   margin-top: 5px;"
        "   background: transparent;"
        "}";
}

#endif // TODODIALOGCONSTANTS_H