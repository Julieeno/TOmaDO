#include "calendar/components/coloreditemdelegate.h"
#include <QPainter>
#include <QStyleOptionViewItem>
#include <QModelIndex>

ColoredItemDelegate::ColoredItemDelegate(QObject* parent)
    : QStyledItemDelegate(parent)
{
}

void ColoredItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QString text = index.data(Qt::DisplayRole).toString();
    QString colorCode = index.data(Qt::UserRole + 1).toString();
    QString itemType = index.data(Qt::UserRole + 2).toString();
    bool isCompleted = index.data(Qt::UserRole + 3).toBool();

    if (!colorCode.isEmpty() || itemType == "todo") {
        painter->save();
        
        // Create a copy of the option and remove focus state
        QStyleOptionViewItem modifiedOption = option;
        modifiedOption.state &= ~QStyle::State_HasFocus;

        if (modifiedOption.state & QStyle::State_Selected) {
            painter->fillRect(modifiedOption.rect, modifiedOption.palette.highlight());
        }

        painter->setRenderHint(QPainter::Antialiasing);

        if (itemType == "todo") {
            paintTodoItem(painter, modifiedOption, index);
        } else {
            paintGoalItem(painter, modifiedOption, index);
        }

        painter->restore();
    } else {
        QStyleOptionViewItem modifiedOption = option;
        modifiedOption.state &= ~QStyle::State_HasFocus;
        QStyledItemDelegate::paint(painter, modifiedOption, index);
    }
}

void ColoredItemDelegate::paintTodoItem(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    bool isCompleted = index.data(Qt::UserRole + 3).toBool();
    QString text = index.data(Qt::DisplayRole).toString();
    
    QRect rect = option.rect;
    QColor circleColor = isCompleted ? QColor("#4CAF50") : QColor("#808080");
    
    paintCircle(painter, rect, circleColor, false);
    
    QColor textColor = (option.state & QStyle::State_Selected) ?
                      option.palette.color(QPalette::HighlightedText) :
                      option.palette.color(QPalette::Text);
    
    QRect textRect = rect.adjusted(CIRCLE_SIZE + TEXT_MARGIN, 0, 0, 0);
    paintText(painter, textRect, text, textColor);
}

void ColoredItemDelegate::paintGoalItem(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QString colorCode = index.data(Qt::UserRole + 1).toString();
    QString text = index.data(Qt::DisplayRole).toString();
    
    QRect rect = option.rect;
    QColor circleColor(colorCode);
    
    paintCircle(painter, rect, circleColor, true);
    
    QColor textColor = (option.state & QStyle::State_Selected) ?
                      option.palette.color(QPalette::HighlightedText) :
                      option.palette.color(QPalette::Text);
    
    QRect textRect = rect.adjusted(CIRCLE_SIZE + TEXT_MARGIN, 0, 0, 0);
    paintText(painter, textRect, text, textColor);
}

void ColoredItemDelegate::paintCircle(QPainter* painter, const QRect& rect, const QColor& color, bool filled) const
{
    int circleX = rect.left() + CIRCLE_MARGIN;
    int circleY = rect.top() + (rect.height() - CIRCLE_SIZE) / 2;
    
    if (filled) {
        painter->setBrush(QBrush(color));
        painter->setPen(QPen(color.darker(120), 1));
    } else {
        painter->setBrush(QBrush(Qt::transparent));
        painter->setPen(QPen(color, 2));
    }
    
    painter->drawEllipse(circleX, circleY, CIRCLE_SIZE, CIRCLE_SIZE);
}

void ColoredItemDelegate::paintText(QPainter* painter, const QRect& rect, const QString& text, const QColor& color) const
{
    painter->setPen(QPen(color));
    painter->drawText(rect, Qt::AlignLeft | Qt::AlignVCenter, text);
}