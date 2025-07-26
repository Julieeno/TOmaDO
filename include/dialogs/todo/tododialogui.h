#ifndef TODODIALOGUI_H
#define TODODIALOGUI_H

#include <QWidget>

class QVBoxLayout;
class QGridLayout;
class QHBoxLayout;
class QLineEdit;
class QTextEdit;
class QComboBox;
class QCheckBox;
class QDateTimeEdit;
class QGroupBox;
class QPushButton;
class QLabel;

class TodoDialogUI {
public:
    struct FormWidgets {
        QLineEdit* titleEdit;
        QTextEdit* descriptionEdit;
        QComboBox* goalCombo;
        QComboBox* priorityCombo;
        QCheckBox* completionCheckbox;
        QCheckBox* hasStartDateCheck;
        QDateTimeEdit* startDateEdit;
        QCheckBox* hasDueDateCheck;
        QDateTimeEdit* dueDateEdit;
        QPushButton* okButton;
        QPushButton* cancelButton;
    };

    static FormWidgets createUI(QWidget* parent);

private:
    static QVBoxLayout* createMainLayout(QWidget* parent);
    static void createBasicFields(QVBoxLayout* layout, QWidget* parent, FormWidgets& widgets);
    static void createGoalSelection(QVBoxLayout* layout, QWidget* parent, FormWidgets& widgets);
    static void createPrioritySelection(QVBoxLayout* layout, QWidget* parent, FormWidgets& widgets);
    static void createCompletionCheckbox(QVBoxLayout* layout, QWidget* parent, FormWidgets& widgets);
    static QGroupBox* createScheduleGroup(QWidget* parent, FormWidgets& widgets);
    static void createButtons(QVBoxLayout* layout, QWidget* parent, FormWidgets& widgets);
    
    static void setupWidgetProperties(FormWidgets& widgets);
    static void applyStyles(QWidget* parent, const FormWidgets& widgets);
    static void setupDateTimeConnections(const FormWidgets& widgets);
    
    static QLabel* createLabel(const QString& text, QWidget* parent);
    static QLabel* createNoteLabel(const QString& text, QWidget* parent);
    
    TodoDialogUI() = delete; // Static utility class
};

#endif // TODODIALOGUI_H