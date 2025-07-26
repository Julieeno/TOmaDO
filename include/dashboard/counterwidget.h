#ifndef COUNTERWIDGET_H
#define COUNTERWIDGET_H

#include <QFrame>
#include <QLabel>
#include <QVBoxLayout>

class CounterWidget : public QFrame {
    Q_OBJECT

public:
    explicit CounterWidget(const QString& title, const QString& value, const QString& color, QWidget* parent = nullptr);
    void updateValue(const QString& value);

private:
    // Known color constants
    static const QString BLUE_COLOR;
    static const QString GREEN_COLOR;
    static const QString ORANGE_COLOR;
    static const QString RED_COLOR;

    // Style constants
    static const QString BASE_STYLE;
    static const QString TITLE_STYLE;
    static const QString VALUE_STYLE;

    void setupUi();
    QString getStyleForColor(const QString& color) const;

    QString m_title;
    QString m_color;
    QLabel* m_titleLabel;
    QLabel* m_valueLabel;
};

#endif // COUNTERWIDGET_H