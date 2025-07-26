#ifndef COLORBUTTON_H
#define COLORBUTTON_H

#include <QPushButton>
#include <QString>
#include "common/colorbuttonrenderer.h"

class ColorButton : public QPushButton {
    Q_OBJECT

public:
    explicit ColorButton(const QString& colorCode, QWidget* parent = nullptr);
    
    QString colorCode() const { return m_colorCode; }
    void setSelected(bool selected);
    bool isSelected() const { return m_isSelected; }

protected:
    void paintEvent(QPaintEvent* event) override;
    void enterEvent(QEvent* event) override;
    void leaveEvent(QEvent* event) override;

private:
    void setupButton();
    void setupStyling();
    void setupTooltip();
    ColorButtonRenderer::ButtonState getCurrentState() const;
    
    QString m_colorCode;
    bool m_isSelected;
    bool m_isHovered;
};

#endif // COLORBUTTON_H