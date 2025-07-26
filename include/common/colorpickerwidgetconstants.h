#ifndef COLORPICKERWIDGETCONSTANTS_H
#define COLORPICKERWIDGETCONSTANTS_H

#include <QColor>
#include <QString>

namespace ColorPickerConstants {
    // Layout constants
    constexpr int BUTTON_SIZE = 40;
    constexpr int BUTTON_MARGIN = 2;
    constexpr int GRID_SPACING = 4;
    constexpr int GRID_MARGINS = 8;
    constexpr int COLUMNS_COUNT = 5;
    
    // Button appearance
    constexpr int BORDER_WIDTH = 2;
    constexpr int SELECTION_BORDER_OFFSET = 1;
    
    // Size calculation constants
    constexpr int BUTTON_TOTAL_SIZE = BUTTON_SIZE + (GRID_SPACING * 2);
    constexpr int WIDGET_PADDING = GRID_MARGINS * 2;
    
    // Default color
    const QString DEFAULT_COLOR = "#3498DB";
    
    // Border colors
    const QString SELECTION_BORDER_COLOR = "#333333";
    const QString HOVER_BORDER_COLOR = "#666666";
}

namespace ColorPickerStyles {
    // Base button style
    const QString COLOR_BUTTON_BASE = 
        "QPushButton {"
        "   border: none;"
        "   border-radius: 20px;"
        "   background: transparent;"
        "}";
    
    // Widget container style
    const QString WIDGET_CONTAINER = 
        "ColorPickerWidget {"
        "   background: rgba(255, 255, 255, 0.95);"
        "   border-radius: 8px;"
        "   padding: 4px;"
        "}";
}

#endif // COLORPICKERWIDGETCONSTANTS_H