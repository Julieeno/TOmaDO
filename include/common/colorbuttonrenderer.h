#ifndef COLORBUTTONRENDERER_H
#define COLORBUTTONRENDERER_H

#include <QPainter>
#include <QRect>
#include <QColor>
#include <QString>

class ColorButtonRenderer {
public:
    enum class ButtonState {
        Normal,
        Hovered,
        Selected
    };
    
    static void renderButton(QPainter* painter, const QRect& rect, 
                           const QString& colorCode, ButtonState state);
    
private:
    static QRect calculateCircleRect(const QRect& buttonRect);
    static void drawColorCircle(QPainter* painter, const QRect& circleRect, const QColor& color);
    static void drawStateBorder(QPainter* painter, const QRect& circleRect, ButtonState state);
    static QColor getBorderColor(ButtonState state);
    static int getBorderWidth(ButtonState state);
    
    ColorButtonRenderer() = delete; // Static utility class
};

#endif // COLORBUTTONRENDERER_H