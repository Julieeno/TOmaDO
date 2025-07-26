#include "dialogs/goal/goaldialogformmanager.h"
#include "dialogs/goal/goaldialogconstants.h"
#include "dialogs/goal/goaldialogvalidator.h"
#include "common/colorpickerwidget.h"
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QCheckBox>

GoalDialogFormManager::GoalDialogFormManager(QWidget* parent)
    : QObject(parent)
    , m_titleEdit(nullptr)
    , m_descriptionEdit(nullptr)
    , m_priorityCombo(nullptr)
    , m_timeSpinBox(nullptr)
    , m_completionCheckbox(nullptr)
    , m_colorPicker(nullptr)
    , m_selectedColor(GoalDialogConstants::DEFAULT_COLOR)
    , m_parentWidget(parent)
{
}

void GoalDialogFormManager::setFormWidgets(QLineEdit* titleEdit, QTextEdit* descriptionEdit, 
                                         QComboBox* priorityCombo, QSpinBox* timeSpinBox, 
                                         QCheckBox* completionCheckbox, ColorPickerWidget* colorPicker)
{
    m_titleEdit = titleEdit;
    m_descriptionEdit = descriptionEdit;
    m_priorityCombo = priorityCombo;
    m_timeSpinBox = timeSpinBox;
    m_completionCheckbox = completionCheckbox;
    m_colorPicker = colorPicker;
    
    setupPriorityComboBox();
    setupTimeSpinBox();
    setupFormConnections();
}

void GoalDialogFormManager::setupPriorityComboBox()
{
    if (!m_priorityCombo) return;
    
    m_priorityCombo->clear();
    m_priorityCombo->addItem(GoalDialogStrings::PRIORITY_NAMES[DatabaseManager::TodoPriority::Low], 
                            static_cast<int>(DatabaseManager::TodoPriority::Low));
    m_priorityCombo->addItem(GoalDialogStrings::PRIORITY_NAMES[DatabaseManager::TodoPriority::Medium], 
                            static_cast<int>(DatabaseManager::TodoPriority::Medium));
    m_priorityCombo->addItem(GoalDialogStrings::PRIORITY_NAMES[DatabaseManager::TodoPriority::High], 
                            static_cast<int>(DatabaseManager::TodoPriority::High));
    
    // Set default to Medium
    m_priorityCombo->setCurrentIndex(1);
}

void GoalDialogFormManager::setupTimeSpinBox()
{
    if (!m_timeSpinBox) return;
    
    m_timeSpinBox->setRange(0, GoalDialogConstants::TIME_INPUT_MAX_VALUE);
    m_timeSpinBox->setValue(GoalDialogConstants::DEFAULT_ESTIMATED_MINUTES);
    m_timeSpinBox->setSuffix(" min");
    m_timeSpinBox->setSpecialValueText("No limit");
}

void GoalDialogFormManager::setupFormConnections()
{
    if (m_colorPicker) {
        connect(m_colorPicker, &ColorPickerWidget::colorSelected,
                this, &GoalDialogFormManager::onColorSelected);
    }
}

void GoalDialogFormManager::populateForm(const DatabaseManager::GoalItem& goal)
{
    if (goal.id == -1) {
        // New goal - use defaults
        m_selectedColor = GoalDialogConstants::DEFAULT_COLOR;
        return;
    }
    
    // Existing goal - populate fields
    if (m_titleEdit) {
        m_titleEdit->setText(goal.title);
    }
    
    if (m_descriptionEdit) {
        m_descriptionEdit->setPlainText(goal.description);
    }
    
    if (m_priorityCombo) {
        int priorityIndex = m_priorityCombo->findData(static_cast<int>(goal.priority));
        if (priorityIndex != -1) {
            m_priorityCombo->setCurrentIndex(priorityIndex);
        }
    }
    
    if (m_timeSpinBox) {
        int minutes = convertSecondsToMinutes(goal.estimatedTime);
        m_timeSpinBox->setValue(minutes);
    }
    
    if (m_completionCheckbox) {
        m_completionCheckbox->setChecked(goal.isCompleted);
    }
    
    m_selectedColor = goal.colorCode.isEmpty() ? GoalDialogConstants::DEFAULT_COLOR : goal.colorCode;
    
    if (m_colorPicker) {
        m_colorPicker->setSelectedColor(m_selectedColor);
    }
}

DatabaseManager::GoalItem GoalDialogFormManager::extractGoalData(const DatabaseManager::GoalItem& originalGoal) const
{
    DatabaseManager::GoalItem goal = originalGoal;
    
    if (m_titleEdit) {
        goal.title = m_titleEdit->text().trimmed();
    }
    
    if (m_descriptionEdit) {
        goal.description = m_descriptionEdit->toPlainText().trimmed();
    }
    
    if (m_priorityCombo) {
        int priorityValue = m_priorityCombo->currentData().toInt();
        goal.priority = static_cast<DatabaseManager::TodoPriority>(priorityValue);
    }
    
    if (m_timeSpinBox) {
        goal.estimatedTime = convertMinutesToSeconds(m_timeSpinBox->value());
    }
    
    if (m_completionCheckbox) {
        goal.isCompleted = m_completionCheckbox->isChecked();
    }
    
    goal.colorCode = m_selectedColor;
    
    return goal;
}

bool GoalDialogFormManager::validateForm()
{
    if (!m_titleEdit || !m_descriptionEdit || !m_timeSpinBox) {
        return false;
    }
    
    QString title = m_titleEdit->text().trimmed();
    QString description = m_descriptionEdit->toPlainText();
    int estimatedTime = m_timeSpinBox->value();
    
    auto result = GoalDialogValidator::validateAll(title, description, estimatedTime,
                                                  m_titleEdit, m_descriptionEdit, m_timeSpinBox);
    
    if (!result.isValid) {
        emit validationFailed(result.errorMessage, result.focusWidget);
        return false;
    }
    
    return true;
}

void GoalDialogFormManager::focusFirstField()
{
    if (m_titleEdit) {
        m_titleEdit->setFocus();
    }
}

void GoalDialogFormManager::onColorSelected(const QString& colorCode)
{
    m_selectedColor = colorCode;
    emit colorChanged(colorCode);
}

int GoalDialogFormManager::convertSecondsToMinutes(int seconds) const
{
    return seconds / GoalDialogConstants::SECONDS_PER_MINUTE;
}

int GoalDialogFormManager::convertMinutesToSeconds(int minutes) const
{
    return minutes * GoalDialogConstants::SECONDS_PER_MINUTE;
}