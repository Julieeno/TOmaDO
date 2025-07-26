#include "dialogs/goal/goaldialogui.h"
#include "dialogs/goal/goaldialogconstants.h"
#include "common/colorpickerwidget.h"
#include "ui/dialogstyles.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QGroupBox>
#include <QDialogButtonBox>
#include <QLabel>

GoalDialogUI::FormWidgets GoalDialogUI::createUI(QWidget* parent)
{
    FormWidgets widgets = {};

    // Create main layout
    QVBoxLayout* mainLayout = createMainLayout(parent);

    // Create form layout with basic fields
    QFormLayout* formLayout = createFormLayout(parent, widgets);
    mainLayout->addLayout(formLayout);

    // Create details group (priority, time, completion)
    QGroupBox* detailsGroup = createDetailsGroup(parent, widgets);
    mainLayout->addWidget(detailsGroup);

    // Create color group
    QGroupBox* colorGroup = createColorGroup(parent, widgets);
    mainLayout->addWidget(colorGroup);

    // Add stretch
    mainLayout->addStretch();

    // Create button box
    widgets.buttonBox = createButtonBox(parent);
    mainLayout->addWidget(widgets.buttonBox);

    // Setup and style widgets
    setupFormWidgets(widgets);
    applyWidgetStyles(widgets);

    return widgets;
}

QVBoxLayout* GoalDialogUI::createMainLayout(QWidget* parent)
{
    QVBoxLayout* layout = new QVBoxLayout(parent);
    layout->setSpacing(GoalDialogConstants::MAIN_LAYOUT_SPACING);
    layout->setContentsMargins(GoalDialogConstants::MAIN_LAYOUT_MARGIN,
                              GoalDialogConstants::MAIN_LAYOUT_MARGIN,
                              GoalDialogConstants::MAIN_LAYOUT_MARGIN,
                              GoalDialogConstants::MAIN_LAYOUT_MARGIN);
    return layout;
}

QFormLayout* GoalDialogUI::createFormLayout(QWidget* parent, FormWidgets& widgets)
{
    QFormLayout* formLayout = new QFormLayout();
    formLayout->setSpacing(GoalDialogConstants::FORM_LAYOUT_SPACING);

    // Title field
    widgets.titleEdit = new QLineEdit(parent);
    widgets.titleEdit->setPlaceholderText(GoalDialogStrings::TITLE_PLACEHOLDER);
    formLayout->addRow(GoalDialogStrings::TITLE_LABEL, widgets.titleEdit);

    // Description field
    widgets.descriptionEdit = new QTextEdit(parent);
    widgets.descriptionEdit->setPlaceholderText(GoalDialogStrings::DESCRIPTION_PLACEHOLDER);
    widgets.descriptionEdit->setMaximumHeight(GoalDialogConstants::DESCRIPTION_MAX_HEIGHT);
    formLayout->addRow(GoalDialogStrings::DESCRIPTION_LABEL, widgets.descriptionEdit);

    return formLayout;
}

QGroupBox* GoalDialogUI::createDetailsGroup(QWidget* parent, FormWidgets& widgets)
{
    QGroupBox* detailsGroup = new QGroupBox(GoalDialogStrings::DETAILS_GROUP_LABEL, parent);
    QFormLayout* detailsLayout = new QFormLayout(detailsGroup);
    detailsLayout->setSpacing(GoalDialogConstants::FORM_LAYOUT_SPACING);

    // Priority combo box
    widgets.priorityCombo = new QComboBox(parent);
    detailsLayout->addRow(GoalDialogStrings::PRIORITY_LABEL, widgets.priorityCombo);

    // Estimated time spin box
    widgets.timeSpinBox = new QSpinBox(parent);
    // Note: QSpinBox doesn't have setPlaceholderText, so we'll set a tooltip instead
    widgets.timeSpinBox->setToolTip(GoalDialogStrings::TIME_PLACEHOLDER);
    detailsLayout->addRow(GoalDialogStrings::ESTIMATED_TIME_LABEL, widgets.timeSpinBox);
    
    // Completion checkbox
    widgets.completionCheckbox = new QCheckBox(GoalDialogStrings::COMPLETION_LABEL, parent);
    detailsLayout->addRow(widgets.completionCheckbox);
    
    return detailsGroup;
}

QGroupBox* GoalDialogUI::createColorGroup(QWidget* parent, FormWidgets& widgets)
{
    QGroupBox* colorGroup = new QGroupBox(GoalDialogStrings::COLOR_GROUP_LABEL, parent);
    QVBoxLayout* colorLayout = new QVBoxLayout(colorGroup);
    colorLayout->setSpacing(GoalDialogConstants::GROUP_LAYOUT_SPACING);
    
    // Color picker widget
    widgets.colorPicker = new ColorPickerWidget(parent);
    colorLayout->addWidget(widgets.colorPicker);
    
    return colorGroup;
}

QDialogButtonBox* GoalDialogUI::createButtonBox(QWidget* parent)
{
    QDialogButtonBox* buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, parent);
    
    DialogStyles::configureButtonBox(buttonBox);
    
    return buttonBox;
}

void GoalDialogUI::setupFormWidgets(FormWidgets& widgets)
{
    // Set maximum lengths
    if (widgets.titleEdit) {
        widgets.titleEdit->setMaxLength(GoalDialogConstants::TITLE_MAX_LENGTH);
    }
    
    // Set focus order
    if (widgets.titleEdit) {
        widgets.titleEdit->setFocus();
    }
}

void GoalDialogUI::applyWidgetStyles(const FormWidgets& widgets)
{
    // Apply priority combo box style
    if (widgets.priorityCombo) {
        widgets.priorityCombo->setStyleSheet(GoalDialogStyles::PRIORITY_COMBOBOX_STYLE);
    }
    
    // Apply time spin box style
    if (widgets.timeSpinBox) {
        widgets.timeSpinBox->setStyleSheet(GoalDialogStyles::TIME_INPUT_STYLE);
    }
    
    // Apply completion checkbox style
    if (widgets.completionCheckbox) {
        widgets.completionCheckbox->setStyleSheet(GoalDialogStyles::COMPLETION_CHECKBOX_STYLE);
    }
}