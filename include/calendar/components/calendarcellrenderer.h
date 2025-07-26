#ifndef CALENDARCELLRENDERER_H
#define CALENDARCELLRENDERER_H

#include <QPainter>
#include <QRect>
#include <QDate>
#include <QColor>
#include <QList>
#include "customcalendarwidget.h"
#include "database/databasemanager.h"

class CalendarCellRenderer {
public:
    // Rendering constants
    static constexpr int MAX_ELEMENTS_PER_CELL = 3;
    static constexpr int CELL_PADDING = 3;
    static constexpr int DATE_TOP_MARGIN = 24;
    static constexpr int INDICATOR_BOTTOM_MARGIN = 22;
    static constexpr int LINE_HEIGHT = 16;
    static constexpr int LINE_SPACING = 2;
    static constexpr int CIRCLE_SIZE = 10;
    static constexpr int CIRCLE_MARGIN = 4;
    static constexpr int TEXT_MARGIN = 8;

    static void renderCell(QPainter* painter, const QRect& rect, const QDate& date,
                          const QList<GoalTimeInfo>& goals, const QList<TodoInfo>& todos);

private:
    static QRect calculateAvailableRect(const QRect& rect);
    static int renderGoalTimeInfo(QPainter* painter, const QRect& rect, 
                                 const QList<GoalTimeInfo>& goalTimes, int startY);
    static void renderMoreIndicator(QPainter* painter, const QRect& rect);
    static void renderGoalItem(QPainter* painter, const QRect& itemRect, 
                              const GoalTimeInfo& goalTime, const QRect& textRect);
    static QString formatGoalTimeText(const GoalTimeInfo& goalTime);
    static QString truncateTextToFit(const QString& text, const QFontMetrics& fm, int maxWidth);
    static QColor getTodoPriorityColor(DatabaseManager::TodoPriority priority);
    
    CalendarCellRenderer() = delete; // Static utility class
};

#endif // CALENDARCELLRENDERER_H