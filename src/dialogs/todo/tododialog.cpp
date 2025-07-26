#include "dialogs/todo/tododialog.h"
#include "dialogs/todo/tododialogformmanager.h"
#include "dialogs/todo/tododialogui.h"
#include "dialogs/todo/tododialogconstants.h"
#include <QMessageBox>
#include <QPushButton>

TodoDialog::TodoDialog(const DatabaseManager::TodoItem& todo, QWidget* parent)
    : QDialog(parent)
    , m_originalTodo(todo)
    , m_preselectedGoalId(-1)
    , m_isEditMode(todo.id != -1)
    , m_formManager(nullptr)
{
    setupDialog();
}

TodoDialog::TodoDialog(int preselectedGoalId, QWidget* parent)
    : QDialog(parent)
    , m_originalTodo(DatabaseManager::TodoItem())
    , m_preselectedGoalId(preselectedGoalId)
    , m_isEditMode(false)
    , m_formManager(nullptr)
{
    setupDialog();
}

void TodoDialog::setupDialog()
{
    setupUI();
    setupFormManager();
    connectSignals();
    configureDialog();
}

void TodoDialog::setupUI()
{
    // Create UI components
    TodoDialogUI::FormWidgets widgets = TodoDialogUI::createUI(this);

    // Create form manager
    m_formManager = new TodoDialogFormManager(this);
    m_formManager->setFormWidgets(widgets.titleEdit, widgets.descriptionEdit,
                                 widgets.goalCombo, widgets.priorityCombo,
                                 widgets.completionCheckbox,
                                 widgets.hasStartDateCheck, widgets.startDateEdit,
                                 widgets.hasDueDateCheck, widgets.dueDateEdit);

    // Connect buttons
    connect(widgets.okButton, &QPushButton::clicked, this, &TodoDialog::onAccept);
    connect(widgets.cancelButton, &QPushButton::clicked, this, &TodoDialog::onCancel);
}

void TodoDialog::setupFormManager()
{
    if (m_formManager) {
        // Populate form with todo data
        m_formManager->populateForm(m_originalTodo);

        // Set preselected goal if specified
        if (m_preselectedGoalId > 0) {
            m_formManager->setPreselectedGoal(m_preselectedGoalId);
        }

        // Set initial focus
        m_formManager->focusFirstField();
    }
}

void TodoDialog::connectSignals()
{
    if (m_formManager) {
        connect(m_formManager, &TodoDialogFormManager::validationFailed,
                this, &TodoDialog::onValidationFailed);
    }
}

void TodoDialog::configureDialog()
{
    // Set dialog properties
    setWindowTitle(getDialogTitle());
    setModal(true);
    setFixedSize(TodoDialogConstants::DIALOG_WIDTH, TodoDialogConstants::DIALOG_HEIGHT);
}

QString TodoDialog::getDialogTitle() const
{
    return m_isEditMode ? TodoDialogStrings::EDIT_TODO_TITLE : TodoDialogStrings::ADD_TODO_TITLE;
}

DatabaseManager::TodoItem TodoDialog::getTodo() const
{
    if (!m_formManager) {
        return m_originalTodo;
    }

    return m_formManager->extractTodoData(m_originalTodo);
}

void TodoDialog::setPreselectedGoal(int goalId, const QString& goalTitle)
{
    m_preselectedGoalId = goalId;

    if (m_formManager) {
        m_formManager->setPreselectedGoal(goalId);
    }

    // goalTitle parameter kept for compatibility but not used in current implementation
    Q_UNUSED(goalTitle);
}

void TodoDialog::onAccept()
{
    if (!m_formManager) {
        reject();
        return;
    }

    if (m_formManager->validateForm()) {
        accept();
    }
    // Validation errors are handled by onValidationFailed slot
}

void TodoDialog::onCancel()
{
    reject();
}

void TodoDialog::onValidationFailed(const QString& message, QWidget* focusWidget)
{
    showValidationError(message, focusWidget);
}

void TodoDialog::showValidationError(const QString& message, QWidget* focusWidget)
{
    QMessageBox::warning(this, TodoDialogConstants::INVALID_INPUT_TITLE, message);

    if (focusWidget) {
        focusWidget->setFocus();
    }
}