#include "common/colorpickerwidget.h"
#include "common/colorbutton.h"
#include "common/colorselectionmanager.h"
#include "common/colorpickergridmanager.h"
#include "common/colorpickerwidgetconstants.h"
#include "database/databasemanager.h"
#include <QGridLayout>
#include <QButtonGroup>

ColorPickerWidget::ColorPickerWidget(QWidget* parent)
    : QWidget(parent)
    , m_gridLayout(nullptr)
    , m_buttonGroup(nullptr)
    , m_selectionManager(nullptr)
{
    setupUi();
}

ColorPickerWidget::~ColorPickerWidget()
{
    // Qt handles cleanup through parent-child relationships
}

void ColorPickerWidget::setupUi()
{
    setupStyling();
    setupLayout();
    createColorButtons();
    configureWidget();
    connectSignals();
}

void ColorPickerWidget::setupStyling()
{
    setStyleSheet(ColorPickerStyles::WIDGET_CONTAINER);
}

void ColorPickerWidget::setupLayout()
{
    m_gridLayout = new QGridLayout(this);

    // Get available colors from database
    m_availableColors = DatabaseManager::instance().getAvailableColors();

    // Configure grid layout
    ColorPickerGridManager::GridInfo gridInfo =
        ColorPickerGridManager::calculateGridInfo(m_availableColors.size());
    ColorPickerGridManager::configureLayout(m_gridLayout, gridInfo);

    // Set minimum size
    setMinimumSize(gridInfo.minimumSize);
}

void ColorPickerWidget::createColorButtons()
{
    // Create button group for exclusive selection
    m_buttonGroup = new QButtonGroup(this);
    m_buttonGroup->setExclusive(true);

    // Create color buttons
    for (const QString& colorCode : m_availableColors) {
        ColorButton* button = new ColorButton(colorCode, this);
        m_colorButtons.append(button);
        m_buttonGroup->addButton(button);
    }

    // Add buttons to grid
    ColorPickerGridManager::GridInfo gridInfo =
        ColorPickerGridManager::calculateGridInfo(m_availableColors.size());
    ColorPickerGridManager::addButtonsToGrid(m_gridLayout, m_colorButtons, gridInfo);
}

void ColorPickerWidget::configureWidget()
{
    // Set size policy to allow expansion
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // Create selection manager
    m_selectionManager = new ColorSelectionManager(this);
    m_selectionManager->setButtons(m_colorButtons);
    m_selectionManager->setButtonGroup(m_buttonGroup);

    // Set default selected color
    m_selectionManager->setSelectedColor(ColorPickerConstants::DEFAULT_COLOR);
}

void ColorPickerWidget::connectSignals()
{
    // Connect selection manager signals
    connect(m_selectionManager, &ColorSelectionManager::colorSelected,
            this, &ColorPickerWidget::colorSelected);
}

void ColorPickerWidget::setSelectedColor(const QString& colorCode)
{
    if (m_selectionManager) {
        m_selectionManager->setSelectedColor(colorCode);
    }
}

QString ColorPickerWidget::selectedColor() const
{
    return m_selectionManager ? m_selectionManager->selectedColor() : QString();
}