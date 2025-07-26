#include "dialogs/todo/tododialogformmanager.h"
#include "dialogs/todo/tododialogconstants.h"
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QDateTimeEdit>
#include <QDateTime>
#include <QTime>

TodoDialogFormManager::TodoDialogFormManager(QWidget* parent)
    : QObject(parent)
    , m_titleEdit(nullptr)
    , m_descriptionEdit(nullptr)
    , m_goalCombo(nullptr)
    , m_priorityCombo(nullptr)
    , m_completionCheckbox(nullptr)
    , m_hasStartDateCheck(nullptr)
    , m_startDateEdit(nullptr)
    , m_hasDueDateCheck(nullptr)
    , m_dueDateEdit(nullptr)
    , m_preselectedGoalId(-1)
    , m_parentWidget(parent)
{
}

void TodoDialogFormManager::setFormWidgets(QLineEdit* titleEdit, QTextEdit* descriptionEdit,
                                         QComboBox* goalCombo, QComboBox* priorityCombo,
                                         QCheckBox* completionCheckbox,
                                         QCheckBox* hasStartDateCheck, QDateTimeEdit* startDateEdit,
                                         QCheckBox* hasDueDateCheck, QDateTimeEdit* dueDateEdit)
{
    m_titleEdit = titleEdit;
    m_descriptionEdit = descriptionEdit;
    m_goalCombo = goalCombo;
    m_priorityCombo = priorityCombo;
    m_completionCheckbox = completionCheckbox;
    m_hasStartDateCheck = hasStartDateCheck;
    m_startDateEdit = startDateEdit;
    m_hasDueDateCheck = hasDueDateCheck;
    m_dueDateEdit = dueDateEdit;

    setupGoalComboBox();
    setupFormConnections();
}

void TodoDialogFormManager::setupGoalComboBox()
{
    if (!m_goalCombo) return;

    populateGoalCombo();
}

void TodoDialogFormManager::setupFormConnections()
{
    if (m_goalCombo) {
        connect(m_goalCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, &TodoDialogFormManager::onGoalSelectionChanged);
    }
}

void TodoDialogFormManager::populateGoalCombo()
{
    if (!m_goalCombo) return;

    m_goalCombo->clear();
    m_goalCombo->addItem(TodoDialogStrings::NO_GOAL_OPTION, 0);

    auto goals = DatabaseManager::instance().getAllGoals(false);
    for (const auto& goal : goals) {
        if (!goal.isCompleted) {
            m_goalCombo->addItem(goal.title, goal.id);
        }
    }
}

void TodoDialogFormManager::populateForm(const DatabaseManager::TodoItem& todo)
{
    if (todo.id == -1) {
        // New todo - set defaults
        if (m_titleEdit) m_titleEdit->clear();
        if (m_descriptionEdit) m_descriptionEdit->clear();
        if (m_priorityCombo) m_priorityCombo->setCurrentIndex(1); // Medium
        if (m_goalCombo) m_goalCombo->setCurrentIndex(0); // No Goal
        if (m_completionCheckbox) m_completionCheckbox->setChecked(false);
        if (m_hasStartDateCheck) m_hasStartDateCheck->setChecked(false);
        if (m_hasDueDateCheck) m_hasDueDateCheck->setChecked(false);
        if (m_startDateEdit) m_startDateEdit->setDateTime(QDateTime::currentDateTime());
        if (m_dueDateEdit) m_dueDateEdit->setDateTime(QDateTime::currentDateTime().addDays(1));
        return;
    }

    // Existing todo - populate fields
    if (m_titleEdit) {
        m_titleEdit->setText(todo.title);
    }

    if (m_descriptionEdit) {
        m_descriptionEdit->setPlainText(todo.description);
    }

    // Set priority
    if (m_priorityCombo) {
        for (int i = 0; i < m_priorityCombo->count(); ++i) {
            if (static_cast<DatabaseManager::TodoPriority>(m_priorityCombo->itemData(i).toInt()) == todo.priority) {
                m_priorityCombo->setCurrentIndex(i);
                break;
            }
        }
    }

    // Set goal selection
    if (m_goalCombo) {
        for (int i = 0; i < m_goalCombo->count(); ++i) {
            if (m_goalCombo->itemData(i).toInt() == todo.goalId) {
                m_goalCombo->setCurrentIndex(i);
                break;
            }
        }
    }

    // Set completion status
    if (m_completionCheckbox) {
        m_completionCheckbox->setChecked(todo.isCompleted);
    }

    // Set dates
    if (m_hasStartDateCheck && m_startDateEdit) {
        if (todo.startDate.isValid()) {
            m_hasStartDateCheck->setChecked(true);
            QDateTime startDateTime(todo.startDate, QTime(9, 0));
            m_startDateEdit->setDateTime(startDateTime);
        } else {
            m_hasStartDateCheck->setChecked(false);
        }
    }

    if (m_hasDueDateCheck && m_dueDateEdit) {
        if (todo.endDate.isValid()) {
            m_hasDueDateCheck->setChecked(true);
            QDateTime endDateTime(todo.endDate, QTime(18, 0));
            m_dueDateEdit->setDateTime(endDateTime);
        } else {
            m_hasDueDateCheck->setChecked(false);
        }
    }
}

DatabaseManager::TodoItem TodoDialogFormManager::extractTodoData(const DatabaseManager::TodoItem& originalTodo) const
{
    DatabaseManager::TodoItem todo = originalTodo;

    if (m_titleEdit) {
        todo.title = m_titleEdit->text().trimmed();
    }

    if (m_descriptionEdit) {
        todo.description = m_descriptionEdit->toPlainText().trimmed();
    }

    if (m_priorityCombo) {
        todo.priority = static_cast<DatabaseManager::TodoPriority>(m_priorityCombo->currentData().toInt());
    }

    if (m_goalCombo) {
        todo.goalId = m_goalCombo->currentData().toInt();
    }

    if (m_completionCheckbox) {
        todo.isCompleted = m_completionCheckbox->isChecked();
    }

    // Set dates based on checkboxes
    if (m_hasStartDateCheck && m_startDateEdit) {
        todo.startDate = m_hasStartDateCheck->isChecked() ? m_startDateEdit->date() : QDate();
    }

    if (m_hasDueDateCheck && m_dueDateEdit) {
        todo.endDate = m_hasDueDateCheck->isChecked() ? m_dueDateEdit->date() : QDate();
    }

    return todo;
}

bool TodoDialogFormManager::validateForm()
{
    if (!validateTitle()) {
        return false;
    }

    if (!validateDates()) {
        return false;
    }

    return true;
}

void TodoDialogFormManager::focusFirstField()
{
    if (m_titleEdit) {
        m_titleEdit->setFocus();
    }
}

void TodoDialogFormManager::setPreselectedGoal(int goalId)
{
    m_preselectedGoalId = goalId;

    if (m_goalCombo && goalId > 0) {
        for (int i = 0; i < m_goalCombo->count(); ++i) {
            if (m_goalCombo->itemData(i).toInt() == goalId) {
                m_goalCombo->setCurrentIndex(i);
                break;
            }
        }
    }
}

void TodoDialogFormManager::onGoalSelectionChanged()
{
    // Handle goal selection change if needed
    // This can be used for future enhancements
}

bool TodoDialogFormManager::validateTitle()
{
    if (!m_titleEdit) {
        return false;
    }

    if (m_titleEdit->text().trimmed().isEmpty()) {
        emit validationFailed(TodoDialogConstants::TITLE_REQUIRED_MESSAGE, m_titleEdit);
        return false;
    }

    return true;
}

bool TodoDialogFormManager::validateDates()
{
    if (!m_hasStartDateCheck || !m_hasDueDateCheck || !m_startDateEdit || !m_dueDateEdit) {
        return true; // Skip validation if widgets are not available
    }

    // Validate date logic only if both dates are enabled
    if (m_hasStartDateCheck->isChecked() && m_hasDueDateCheck->isChecked()) {
        if (m_startDateEdit->dateTime() >= m_dueDateEdit->dateTime()) {
            emit validationFailed(TodoDialogConstants::DATE_ERROR_MESSAGE, m_dueDateEdit);
            return false;
        }
    }

    return true;
}