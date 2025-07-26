#include "dialogs/goal/goaldialog.h"
#include "dialogs/goal/goaldialogformmanager.h"
#include "dialogs/goal/goaldialogui.h"
#include "dialogs/goal/goaldialogconstants.h"
#include "ui/dialogstyles.h"
#include <QMessageBox>
#include <QDialogButtonBox>

GoalDialog::GoalDialog(const DatabaseManager::GoalItem& goal, QWidget* parent)
    : QDialog(parent)
    , m_originalGoal(goal)
    , m_formManager(nullptr)
    , m_isEditMode(goal.id != -1)
{
    setupDialog();
}

GoalDialog::~GoalDialog()
{
    // Form manager cleanup handled by parent-child relationship
}

void GoalDialog::setupDialog()
{
    setupUI();
    setupFormManager();
    connectSignals();
    configureDialog();
}

void GoalDialog::setupUI()
{
    // Create UI components
    GoalDialogUI::FormWidgets widgets = GoalDialogUI::createUI(this);

    // Store form manager with widgets
    m_formManager = new GoalDialogFormManager(this);
    m_formManager->setFormWidgets(widgets.titleEdit, widgets.descriptionEdit,
                                widgets.priorityCombo, widgets.timeSpinBox,
                                widgets.completionCheckbox, widgets.colorPicker);

    // Connect button box
    connect(widgets.buttonBox, &QDialogButtonBox::accepted, this, &GoalDialog::onAccept);
    connect(widgets.buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

void GoalDialog::setupFormManager()
{
    if (m_formManager) {
        // Populate form with goal data
        m_formManager->populateForm(m_originalGoal);

        // Set initial focus
        m_formManager->focusFirstField();
    }
}

void GoalDialog::connectSignals()
{
    if (m_formManager) {
        connect(m_formManager, &GoalDialogFormManager::colorChanged,
                this, &GoalDialog::onColorChanged);
        connect(m_formManager, &GoalDialogFormManager::validationFailed,
                this, &GoalDialog::onValidationFailed);
    }
}

void GoalDialog::configureDialog()
{
    // Set dialog properties
    setWindowTitle(getDialogTitle());
    setModal(true);
    resize(GoalDialogConstants::DIALOG_WIDTH, GoalDialogConstants::DIALOG_HEIGHT);

    // Apply unified dialog styling
    setStyleSheet(DialogStyles::getDialogStyle());
}

QString GoalDialog::getDialogTitle() const
{
    return m_isEditMode ? GoalDialogStrings::EDIT_GOAL_TITLE : GoalDialogStrings::ADD_GOAL_TITLE;
}

void GoalDialog::onAccept()
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

void GoalDialog::onColorChanged(const QString& colorCode)
{
    // Color change handled by form manager
    // This slot can be used for additional color-related logic if needed
    Q_UNUSED(colorCode);
}

void GoalDialog::onValidationFailed(const QString& message, QWidget* focusWidget)
{
    showValidationError(message, focusWidget);
}

void GoalDialog::showValidationError(const QString& message, QWidget* focusWidget)
{
    QMessageBox::warning(this, GoalDialogConstants::INVALID_INPUT_TITLE, message);

    if (focusWidget) {
        focusWidget->setFocus();
    }
}

DatabaseManager::GoalItem GoalDialog::getGoal() const
{
    if (!m_formManager) {
        return m_originalGoal;
    }

    return m_formManager->extractGoalData(m_originalGoal);
}