#include "common/colorbutton.h"
#include "common/colorbuttonrenderer.h"
#include "common/colorpickerwidgetconstants.h"
#include "database/databasemanager.h"
#include <QPaintEvent>
#include <QEvent>

ColorButton::ColorButton(const QString& colorCode, QWidget* parent)
    : QPushButton(parent)
    , m_colorCode(colorCode)
    , m_isSelected(false)
    , m_isHovered(false)
{
    setupButton();
    setupStyling();
    setupTooltip();
}

void ColorButton::setupButton()
{
    setFixedSize(ColorPickerConstants::BUTTON_SIZE, ColorPickerConstants::BUTTON_SIZE);
    setCheckable(true);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

void ColorButton::setupStyling()
{
    setStyleSheet(ColorPickerStyles::COLOR_BUTTON_BASE);
}

void ColorButton::setupTooltip()
{
    QString colorName = DatabaseManager::instance().getColorName(m_colorCode);
    setToolTip(colorName);
}

void ColorButton::setSelected(bool selected)
{
    if (m_isSelected != selected) {
        m_isSelected = selected;
        update();
    }
}

void ColorButton::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    
    QPainter painter(this);
    ColorButtonRenderer::ButtonState state = getCurrentState();
    
    ColorButtonRenderer::renderButton(&painter, rect(), m_colorCode, state);
}

void ColorButton::enterEvent(QEvent* event)
{
    QPushButton::enterEvent(event);
    m_isHovered = true;
    update();
}

void ColorButton::leaveEvent(QEvent* event)
{
    QPushButton::leaveEvent(event);
    m_isHovered = false;
    update();
}

ColorButtonRenderer::ButtonState ColorButton::getCurrentState() const
{
    if (m_isSelected) {
        return ColorButtonRenderer::ButtonState::Selected;
    } else if (m_isHovered) {
        return ColorButtonRenderer::ButtonState::Hovered;
    } else {
        return ColorButtonRenderer::ButtonState::Normal;
    }
}