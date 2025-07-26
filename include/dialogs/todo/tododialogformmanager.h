#ifndef TODODIALOGFORMMANAGER_H
#define TODODIALOGFORMMANAGER_H

#include <QObject>
#include "database/databasemanager.h"

class QLineEdit;
class QTextEdit;
class QComboBox;
class QCheckBox;
class QDateTimeEdit;
class QWidget;

class TodoDialogFormManager : public QObject {
    Q_OBJECT

public:
    explicit TodoDialogFormManager(QWidget* parent = nullptr);

    void setFormWidgets(QLineEdit* titleEdit, QTextEdit* descriptionEdit,
                       QComboBox* goalCombo, QComboBox* priorityCombo,
                       QCheckBox* completionCheckbox,
                       QCheckBox* hasStartDateCheck, QDateTimeEdit* startDateEdit,
                       QCheckBox* hasDueDateCheck, QDateTimeEdit* dueDateEdit);

    void populateForm(const DatabaseManager::TodoItem& todo);
    DatabaseManager::TodoItem extractTodoData(const DatabaseManager::TodoItem& originalTodo) const;

    bool validateForm();
    void focusFirstField();

    void setPreselectedGoal(int goalId);

    signals:
        void validationFailed(const QString& message, QWidget* focusWidget);

private slots:
    void onGoalSelectionChanged();

private:
    void setupGoalComboBox();
    void setupFormConnections();
    void populateGoalCombo();

    bool validateTitle();
    bool validateDates();

    // Form widgets
    QLineEdit* m_titleEdit;
    QTextEdit* m_descriptionEdit;
    QComboBox* m_goalCombo;
    QComboBox* m_priorityCombo;
    QCheckBox* m_completionCheckbox;
    QCheckBox* m_hasStartDateCheck;
    QDateTimeEdit* m_startDateEdit;
    QCheckBox* m_hasDueDateCheck;
    QDateTimeEdit* m_dueDateEdit;

    // State
    int m_preselectedGoalId;
    QWidget* m_parentWidget;
};

#endif // TODODIALOGFORMMANAGER_H