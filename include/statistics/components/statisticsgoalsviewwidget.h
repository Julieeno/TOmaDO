#ifndef STATISTICSGOALSVIEWWIDGET_H
#define STATISTICSGOALSVIEWWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QColor>

class StatisticsGoalsViewWidget : public QWidget {
    Q_OBJECT

public:
    explicit StatisticsGoalsViewWidget(QWidget* parent = nullptr);
    
    void clearBars();
    void addBar(const QString& label, double hours, const QColor& color = QColor("#FF6B6B"));
    void setSummaryText(const QString& text);
    void setMaxHours(double maxHours) { m_maxHours = maxHours; }

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    void setupUi();
    
    QVBoxLayout* m_mainLayout;
    QLabel* m_titleLabel;
    QLabel* m_summaryLabel;
    QScrollArea* m_scrollArea;
    QWidget* m_barsContainer;
    QVBoxLayout* m_barsLayout;
    
    double m_maxHours;
};

#endif // STATISTICSGOALSVIEWWIDGET_H