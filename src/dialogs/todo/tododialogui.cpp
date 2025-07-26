#include "dialogs/todo/tododialogui.h"
#include "dialogs/todo/tododialogconstants.h"
#include <QVBoxLayout>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QDateTimeEdit>
#include <QGroupBox>
#include <QPushButton>
#include <QLabel>
#include <QDateTime>

TodoDialogUI::FormWidgets TodoDialogUI::createUI(QWidget* parent)
{
    FormWidgets widgets = {};

    QVBoxLayout* mainLayout = createMainLayout(parent);

    createBasicFields(mainLayout, parent, widgets);
    createGoalSelection(mainLayout, parent, widgets);
    createPrioritySelection(mainLayout, parent, widgets);
    createCompletionCheckbox(mainLayout, parent, widgets);

    QGroupBox* scheduleGroup = createScheduleGroup(parent, widgets);
    mainLayout->addWidget(scheduleGroup);

    createButtons(mainLayout, parent, widgets);

    setupWidgetProperties(widgets);
    applyStyles(parent, widgets);
    setupDateTimeConnections(widgets);

    return widgets;
}

QVBoxLayout* TodoDialogUI::createMainLayout(QWidget* parent)
{
    auto layout = new QVBoxLayout(parent);
    layout->setContentsMargins(TodoDialogConstants::MAIN_LAYOUT_MARGIN,
                              TodoDialogConstants::MAIN_LAYOUT_MARGIN,
                              TodoDialogConstants::MAIN_LAYOUT_MARGIN,
                              TodoDialogConstants::MAIN_LAYOUT_MARGIN);
    layout->setSpacing(TodoDialogConstants::MAIN_LAYOUT_SPACING);
    return layout;
}

void TodoDialogUI::createBasicFields(QVBoxLayout* layout, QWidget* parent, FormWidgets& widgets)
{
    // Title field
    layout->addWidget(createLabel(TodoDialogStrings::TITLE_LABEL, parent));
    widgets.titleEdit = new QLineEdit(parent);
    widgets.titleEdit->setPlaceholderText(TodoDialogStrings::TITLE_PLACEHOLDER);
    widgets.titleEdit->setMaxLength(TodoDialogConstants::TITLE_MAX_LENGTH);
    layout->addWidget(widgets.titleEdit);

    // Description field
    layout->addWidget(createLabel(TodoDialogStrings::DESCRIPTION_LABEL, parent));
    widgets.descriptionEdit = new QTextEdit(parent);
    widgets.descriptionEdit->setPlaceholderText(TodoDialogStrings::DESCRIPTION_PLACEHOLDER);
    widgets.descriptionEdit->setMaximumHeight(TodoDialogConstants::DESCRIPTION_MAX_HEIGHT);
    layout->addWidget(widgets.descriptionEdit);
}

void TodoDialogUI::createGoalSelection(QVBoxLayout* layout, QWidget* parent, FormWidgets& widgets)
{
    layout->addWidget(createLabel(TodoDialogStrings::GOAL_LABEL, parent));
    widgets.goalCombo = new QComboBox(parent);
    layout->addWidget(widgets.goalCombo);
}

void TodoDialogUI::createPrioritySelection(QVBoxLayout* layout, QWidget* parent, FormWidgets& widgets)
{
    layout->addWidget(createLabel(TodoDialogStrings::PRIORITY_LABEL, parent));
    widgets.priorityCombo = new QComboBox(parent);
    layout->addWidget(widgets.priorityCombo);
}

void TodoDialogUI::createCompletionCheckbox(QVBoxLayout* layout, QWidget* parent, FormWidgets& widgets)
{
    widgets.completionCheckbox = new QCheckBox(TodoDialogStrings::COMPLETION_LABEL, parent);
    layout->addWidget(widgets.completionCheckbox);
}

QGroupBox* TodoDialogUI::createScheduleGroup(QWidget* parent, FormWidgets& widgets)
{
    auto groupBox = new QGroupBox(TodoDialogStrings::SCHEDULE_GROUP_LABEL, parent);
    auto layout = new QGridLayout(groupBox);
    layout->setSpacing(TodoDialogConstants::GROUP_LAYOUT_SPACING);

    // Start date row
    widgets.hasStartDateCheck = new QCheckBox(TodoDialogStrings::START_DATE_LABEL, parent);
    layout->addWidget(widgets.hasStartDateCheck, 0, 0);

    widgets.startDateEdit = new QDateTimeEdit(parent);
    widgets.startDateEdit->setEnabled(false);
    layout->addWidget(widgets.startDateEdit, 0, 1);

    // Due date row
    widgets.hasDueDateCheck = new QCheckBox(TodoDialogStrings::DUE_DATE_LABEL, parent);
    layout->addWidget(widgets.hasDueDateCheck, 1, 0);

    widgets.dueDateEdit = new QDateTimeEdit(parent);
    widgets.dueDateEdit->setEnabled(false);
    layout->addWidget(widgets.dueDateEdit, 1, 1);

    // Note label
    auto noteLabel = createNoteLabel(TodoDialogStrings::SCHEDULE_NOTE, parent);
    layout->addWidget(noteLabel, 2, 0, 1, 2);

    return groupBox;
}

void TodoDialogUI::createButtons(QVBoxLayout* layout, QWidget* parent, FormWidgets& widgets)
{
    auto buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();

    widgets.cancelButton = new QPushButton(TodoDialogStrings::CANCEL_BUTTON, parent);
    widgets.cancelButton->setObjectName("cancelButton");
    buttonLayout->addWidget(widgets.cancelButton);

    widgets.okButton = new QPushButton(TodoDialogStrings::SAVE_BUTTON, parent);
    buttonLayout->addWidget(widgets.okButton);

    layout->addLayout(buttonLayout);
}

void TodoDialogUI::setupWidgetProperties(FormWidgets& widgets)
{
    // Priority combo
    if (widgets.priorityCombo) {
        widgets.priorityCombo->addItem(TodoDialogStrings::PRIORITY_NAMES[DatabaseManager::TodoPriority::Low],
                                      static_cast<int>(DatabaseManager::TodoPriority::Low));
        widgets.priorityCombo->addItem(TodoDialogStrings::PRIORITY_NAMES[DatabaseManager::TodoPriority::Medium],
                                      static_cast<int>(DatabaseManager::TodoPriority::Medium));
        widgets.priorityCombo->addItem(TodoDialogStrings::PRIORITY_NAMES[DatabaseManager::TodoPriority::High],
                                      static_cast<int>(DatabaseManager::TodoPriority::High));
        widgets.priorityCombo->setCurrentIndex(1); // Default to Medium
    }

    // Date time edits
    if (widgets.startDateEdit) {
        widgets.startDateEdit->setDateTime(QDateTime::currentDateTime());
        widgets.startDateEdit->setCalendarPopup(true);
        widgets.startDateEdit->setDisplayFormat(TodoDialogConstants::DATE_TIME_FORMAT);
    }

    if (widgets.dueDateEdit) {
        widgets.dueDateEdit->setDateTime(QDateTime::currentDateTime().addDays(7));
        widgets.dueDateEdit->setCalendarPopup(true);
        widgets.dueDateEdit->setDisplayFormat(TodoDialogConstants::DATE_TIME_FORMAT);
    }
}

void TodoDialogUI::applyStyles(QWidget* parent, const FormWidgets& widgets)
{
    QString combinedStyle = TodoDialogStyles::MAIN_DIALOG_STYLE +
                           TodoDialogStyles::FORM_LABEL_STYLE +
                           TodoDialogStyles::INPUT_FIELD_STYLE +
                           TodoDialogStyles::COMBOBOX_STYLE +
                           TodoDialogStyles::DATETIME_STYLE +
                           TodoDialogStyles::CHECKBOX_STYLE +
                           TodoDialogStyles::BUTTON_STYLE +
                           TodoDialogStyles::GROUPBOX_STYLE +
                           TodoDialogStyles::NOTE_LABEL_STYLE;
    
    parent->setStyleSheet(combinedStyle);
}

void TodoDialogUI::setupDateTimeConnections(const FormWidgets& widgets)
{
    // Enable/disable date editors based on checkboxes
    if (widgets.hasStartDateCheck && widgets.startDateEdit) {
        QObject::connect(widgets.hasStartDateCheck, &QCheckBox::toggled, 
                        widgets.startDateEdit, &QDateTimeEdit::setEnabled);
    }
    
    if (widgets.hasDueDateCheck && widgets.dueDateEdit) {
        QObject::connect(widgets.hasDueDateCheck, &QCheckBox::toggled, 
                        widgets.dueDateEdit, &QDateTimeEdit::setEnabled);
    }
    
    // Smart date suggestions
    if (widgets.hasStartDateCheck && widgets.hasDueDateCheck && widgets.dueDateEdit) {
        QObject::connect(widgets.hasStartDateCheck, &QCheckBox::toggled, 
                        [widgets](bool checked) {
            if (checked && !widgets.hasDueDateCheck->isChecked()) {
                widgets.dueDateEdit->setDateTime(widgets.startDateEdit->dateTime().addDays(1));
            }
        });
    }
    
    // Auto-adjust due date when start date changes
    if (widgets.startDateEdit && widgets.dueDateEdit && widgets.hasStartDateCheck && widgets.hasDueDateCheck) {
        QObject::connect(widgets.startDateEdit, &QDateTimeEdit::dateTimeChanged, 
                        [widgets](const QDateTime& startTime) {
            if (widgets.hasStartDateCheck->isChecked() && widgets.hasDueDateCheck->isChecked()) {
                if (widgets.dueDateEdit->dateTime() <= startTime) {
                    widgets.dueDateEdit->setDateTime(startTime.addSecs(3600)); // Add 1 hour
                }
            }
        });
    }
}

QLabel* TodoDialogUI::createLabel(const QString& text, QWidget* parent)
{
    auto label = new QLabel(text, parent);
    label->setStyleSheet("font-weight: bold;");
    return label;
}

QLabel* TodoDialogUI::createNoteLabel(const QString& text, QWidget* parent)
{
    auto label = new QLabel(text, parent);
    // Note style applied via stylesheet
    return label;
}