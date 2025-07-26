#include "common/colorpickergridmanager.h"
#include "common/colorpickerwidgetconstants.h"
#include "common/colorbutton.h"
#include <QGridLayout>
#include <algorithm>

ColorPickerGridManager::GridInfo ColorPickerGridManager::calculateGridInfo(int colorCount)
{
    GridInfo info;
    info.totalColumns = ColorPickerConstants::COLUMNS_COUNT;
    info.totalRows = (colorCount + info.totalColumns - 1) / info.totalColumns;
    info.minimumSize = calculateMinimumWidgetSize(info);
    
    return info;
}

void ColorPickerGridManager::configureLayout(QGridLayout* layout, const GridInfo& info)
{
    layout->setSpacing(ColorPickerConstants::GRID_SPACING);
    layout->setContentsMargins(ColorPickerConstants::GRID_MARGINS, 
                              ColorPickerConstants::GRID_MARGINS,
                              ColorPickerConstants::GRID_MARGINS, 
                              ColorPickerConstants::GRID_MARGINS);
    
    // Configure column stretching
    for (int i = 0; i < info.totalColumns; ++i) {
        layout->setColumnStretch(i, 1);
    }
    
    // Configure row stretching
    for (int i = 0; i < info.totalRows; ++i) {
        layout->setRowStretch(i, 1);
    }
}

void ColorPickerGridManager::addButtonsToGrid(QGridLayout* layout, const QList<ColorButton*>& buttons, 
                                            const GridInfo& info)
{
    int row = 0, col = 0;
    
    for (ColorButton* button : buttons) {
        layout->addWidget(button, row, col);
        
        if (++col >= info.totalColumns) {
            col = 0;
            row++;
        }
    }
}

QSize ColorPickerGridManager::calculateMinimumWidgetSize(const GridInfo& info)
{
    int minWidth = info.totalColumns * ColorPickerConstants::BUTTON_TOTAL_SIZE + 
                   (info.totalColumns - 1) * ColorPickerConstants::GRID_SPACING + 
                   ColorPickerConstants::WIDGET_PADDING;
    
    int minHeight = info.totalRows * ColorPickerConstants::BUTTON_TOTAL_SIZE + 
                    (info.totalRows - 1) * ColorPickerConstants::GRID_SPACING + 
                    ColorPickerConstants::WIDGET_PADDING;
    
    return QSize(minWidth, minHeight);
}