#ifndef COLORPICKERGRIDMANAGER_H
#define COLORPICKERGRIDMANAGER_H

#include <QSize>
#include <QGridLayout>
#include <QWidget>
#include <QList>
#include <QString>

class ColorButton;

class ColorPickerGridManager {
public:
    struct GridInfo {
        int totalRows;
        int totalColumns;
        QSize minimumSize;
    };
    
    static GridInfo calculateGridInfo(int colorCount);
    static void configureLayout(QGridLayout* layout, const GridInfo& info);
    static void addButtonsToGrid(QGridLayout* layout, const QList<ColorButton*>& buttons, 
                                const GridInfo& info);
    static QSize calculateMinimumWidgetSize(const GridInfo& info);
    
private:
    ColorPickerGridManager() = delete; // Static utility class
};

#endif // COLORPICKERGRIDMANAGER_H