#include "calendar/components/calendarcellrenderer.h"
#include <QPainter>
#include <QFont>
#include <QFontMetrics>

void CalendarCellRenderer::renderCell(QPainter* painter, const QRect& rect, const QDate& date,
                                     const QList<GoalTimeInfo>& goals, const QList<TodoInfo>& todos)
{
    if (goals.isEmpty() && todos.isEmpty()) {
        return;
    }

    QRect availableRect = calculateAvailableRect(rect);
    int totalElements = goals.size() + todos.size();
    int goalsToShow = qMin(goals.size(), MAX_ELEMENTS_PER_CELL);
    
    // Render goals
    int currentY = availableRect.top();
    if (!goals.isEmpty()) {
        currentY = renderGoalTimeInfo(painter, availableRect, goals.mid(0, goalsToShow), currentY);
    }

    // Render more indicator if needed
    if (totalElements > MAX_ELEMENTS_PER_CELL) {
        renderMoreIndicator(painter, availableRect);
    }
}

QRect CalendarCellRenderer::calculateAvailableRect(const QRect& rect)
{
    return rect.adjusted(CELL_PADDING, DATE_TOP_MARGIN, -CELL_PADDING, -CELL_PADDING);
}

int CalendarCellRenderer::renderGoalTimeInfo(QPainter* painter, const QRect& rect, 
                                           const QList<GoalTimeInfo>& goalTimes, int startY)
{
    if (goalTimes.isEmpty()) {
        return startY;
    }

    painter->save();
    
    QFont textFont = painter->font();
    textFont.setPointSize(11);
    painter->setFont(textFont);

    int currentY = startY;
    for (const auto& goalTime : goalTimes) {
        // Check if we have enough space
        if (currentY + LINE_HEIGHT > rect.bottom() - INDICATOR_BOTTOM_MARGIN) {
            break;
        }

        QRect itemRect(rect.left(), currentY, rect.width(), LINE_HEIGHT);
        QRect textRect(rect.left() + CIRCLE_SIZE + TEXT_MARGIN, currentY, 
                      rect.width() - CIRCLE_SIZE - TEXT_MARGIN * 2, LINE_HEIGHT);
        
        renderGoalItem(painter, itemRect, goalTime, textRect);
        currentY += LINE_HEIGHT + LINE_SPACING;
    }

    painter->restore();
    return currentY;
}

void CalendarCellRenderer::renderGoalItem(QPainter* painter, const QRect& itemRect, 
                                        const GoalTimeInfo& goalTime, const QRect& textRect)
{
    // Draw background
    painter->setPen(Qt::NoPen);
    painter->setBrush(QBrush(QColor(220, 220, 220, 180)));
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->drawRoundedRect(itemRect, 6, 6);

    // Draw colored circle
    int circleX = itemRect.left() + CIRCLE_MARGIN;
    int circleY = itemRect.top() + (LINE_HEIGHT - CIRCLE_SIZE) / 2;
    QRect circleRect(circleX, circleY, CIRCLE_SIZE, CIRCLE_SIZE);
    painter->setPen(QPen(goalTime.goalColor.darker(120), 1));
    painter->setBrush(QBrush(goalTime.goalColor));
    painter->drawEllipse(circleRect);

    // Draw text
    painter->setPen(QPen(Qt::black));
    painter->setRenderHint(QPainter::TextAntialiasing, true);
    
    QString timeText = formatGoalTimeText(goalTime);
    QString fullText = QString("%1 (%2)").arg(goalTime.goalTitle).arg(timeText);
    
    QFontMetrics fm(painter->font());
    QString displayText = truncateTextToFit(fullText, fm, textRect.width());
    
    painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, displayText);
}

QString CalendarCellRenderer::formatGoalTimeText(const GoalTimeInfo& goalTime)
{
    if (goalTime.minutes >= 60) {
        int hours = goalTime.minutes / 60;
        int mins = goalTime.minutes % 60;
        return mins > 0 ? QString("%1h%2m").arg(hours).arg(mins) : QString("%1h").arg(hours);
    } else {
        return QString("%1m").arg(goalTime.minutes);
    }
}

QString CalendarCellRenderer::truncateTextToFit(const QString& text, const QFontMetrics& fm, int maxWidth)
{
    if (fm.horizontalAdvance(text) <= maxWidth) {
        return text;
    }
    
    // Try to find a good truncation point
    int textWidth = fm.horizontalAdvance(text);
    if (textWidth <= maxWidth) {
        return text;
    }
    
    // Simple truncation - could be improved with ellipsis
    int chars = text.length() * maxWidth / textWidth;
    return text.left(qMax(1, chars - 3)) + "...";
}

void CalendarCellRenderer::renderMoreIndicator(QPainter* painter, const QRect& rect)
{
    painter->save();

    QRect moreRect = rect;
    moreRect.setTop(moreRect.bottom() - 20);
    moreRect.setHeight(16);

    QFont moreFont = painter->font();
    moreFont.setPointSize(18);
    moreFont.setBold(true);
    painter->setFont(moreFont);
    painter->setPen(QPen(QColor("#e74c3c"), 2));
    painter->setRenderHint(QPainter::TextAntialiasing, true);
    painter->drawText(moreRect, Qt::AlignCenter, "⋯");

    painter->restore();
}

QColor CalendarCellRenderer::getTodoPriorityColor(DatabaseManager::TodoPriority priority)
{
    switch (priority) {
        case DatabaseManager::TodoPriority::High:
            return QColor("#FF6B6B");
        case DatabaseManager::TodoPriority::Medium:
            return QColor("#FFD93D");
        case DatabaseManager::TodoPriority::Low:
            return QColor("#6BCF7F");
        default:
            return QColor("#CCCCCC");
    }
}