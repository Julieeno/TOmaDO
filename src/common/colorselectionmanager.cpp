#include "common/colorselectionmanager.h"
#include "common/colorbutton.h"
#include "common/colorpickerwidgetconstants.h"
#include <QButtonGroup>

ColorSelectionManager::ColorSelectionManager(QObject* parent)
    : QObject(parent)
    , m_buttonGroup(nullptr)
    , m_selectedColor(ColorPickerConstants::DEFAULT_COLOR)
{
}

void ColorSelectionManager::setButtons(const QList<ColorButton*>& buttons)
{
    m_colorButtons = buttons;
    
    // Connect button signals
    for (ColorButton* button : m_colorButtons) {
        connect(button, &QPushButton::clicked, this, &ColorSelectionManager::onColorButtonClicked);
    }
}

void ColorSelectionManager::setButtonGroup(QButtonGroup* buttonGroup)
{
    m_buttonGroup = buttonGroup;
}

void ColorSelectionManager::setSelectedColor(const QString& colorCode)
{
    if (m_selectedColor != colorCode) {
        m_selectedColor = colorCode;
        updateButtonSelection(colorCode);
    }
}

void ColorSelectionManager::onColorButtonClicked()
{
    ColorButton* button = qobject_cast<ColorButton*>(sender());
    if (button) {
        QString newColor = button->colorCode();
        setSelectedColor(newColor);
        emit colorSelected(newColor);
    }
}

void ColorSelectionManager::updateButtonSelection(const QString& colorCode)
{
    for (ColorButton* button : m_colorButtons) {
        bool isSelected = (button->colorCode() == colorCode);
        button->setSelected(isSelected);
        button->setChecked(isSelected);
    }
}

ColorButton* ColorSelectionManager::findButtonByColor(const QString& colorCode) const
{
    for (ColorButton* button : m_colorButtons) {
        if (button->colorCode() == colorCode) {
            return button;
        }
    }
    return nullptr;
}