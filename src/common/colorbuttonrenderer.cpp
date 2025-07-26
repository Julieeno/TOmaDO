#include "common/colorbuttonrenderer.h"
#include "common/colorpickerwidgetconstants.h"
#include <QPainter>
#include <QPen>
#include <QBrush>

void ColorButtonRenderer::renderButton(QPainter* painter, const QRect& rect, 
                                     const QString& colorCode, ButtonState state)
{
    painter->setRenderHint(QPainter::Antialiasing);
    
    QRect circleRect = calculateCircleRect(rect);
    QColor color(colorCode);
    
    drawColorCircle(painter, circleRect, color);
    drawStateBorder(painter, circleRect, state);
}

QRect ColorButtonRenderer::calculateCircleRect(const QRect& buttonRect)
{
    int size = qMin(buttonRect.width(), buttonRect.height()) - 
               (ColorPickerConstants::BUTTON_MARGIN * 2);
    int x = buttonRect.x() + (buttonRect.width() - size) / 2;
    int y = buttonRect.y() + (buttonRect.height() - size) / 2;
    
    return QRect(x, y, size, size);
}

void ColorButtonRenderer::drawColorCircle(QPainter* painter, const QRect& circleRect, const QColor& color)
{
    painter->setBrush(QBrush(color));
    painter->setPen(Qt::NoPen);
    painter->drawEllipse(circleRect);
}

void ColorButtonRenderer::drawStateBorder(QPainter* painter, const QRect& circleRect, ButtonState state)
{
    if (state == ButtonState::Normal) {
        return;
    }
    
    QColor borderColor = getBorderColor(state);
    int borderWidth = getBorderWidth(state);
    
    painter->setPen(QPen(borderColor, borderWidth));
    painter->setBrush(Qt::NoBrush);
    
    QRect borderRect = circleRect.adjusted(
        -ColorPickerConstants::SELECTION_BORDER_OFFSET,
        -ColorPickerConstants::SELECTION_BORDER_OFFSET,
        ColorPickerConstants::SELECTION_BORDER_OFFSET,
        ColorPickerConstants::SELECTION_BORDER_OFFSET
    );
    
    painter->drawEllipse(borderRect);
}

QColor ColorButtonRenderer::getBorderColor(ButtonState state)
{
    switch (state) {
        case ButtonState::Selected:
            return QColor(ColorPickerConstants::SELECTION_BORDER_COLOR);
        case ButtonState::Hovered:
            return QColor(ColorPickerConstants::HOVER_BORDER_COLOR);
        default:
            return QColor();
    }
}

int ColorButtonRenderer::getBorderWidth(ButtonState state)
{
    return ColorPickerConstants::BORDER_WIDTH;
}