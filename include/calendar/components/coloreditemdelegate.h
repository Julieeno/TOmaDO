#ifndef COLOREDITEMDELEGATE_H
#define COLOREDITEMDELEGATE_H

#include <QStyledItemDelegate>

class ColoredItemDelegate : public QStyledItemDelegate {
    Q_OBJECT

public:
    explicit ColoredItemDelegate(QObject* parent = nullptr);
    
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

private:
    void paintTodoItem(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
    void paintGoalItem(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
    void paintCircle(QPainter* painter, const QRect& rect, const QColor& color, bool filled = true) const;
    void paintText(QPainter* painter, const QRect& rect, const QString& text, const QColor& color) const;
    
    static constexpr int CIRCLE_SIZE = 12;
    static constexpr int CIRCLE_MARGIN = 8;
    static constexpr int TEXT_MARGIN = 16;
};

#endif // COLOREDITEMDELEGATE_H