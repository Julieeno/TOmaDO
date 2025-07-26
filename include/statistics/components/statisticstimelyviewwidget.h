#ifndef STATISTICSTIMELYVIEWWIDGET_H
#define STATISTICSTIMELYVIEWWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QList>
#include "statistics/statisticswidget.h"

class StatisticsTimelyViewWidget : public QWidget {
    Q_OBJECT

public:
    explicit StatisticsTimelyViewWidget(QWidget* parent = nullptr);
    
    void clearVerticalBars();
    void addVerticalBar(const QString& label, double maxHours, 
                       const QList<StatisticsVerticalBarWidget::Segment>& segments);
    void setSummaryText(const QString& text);
    void setMaxVerticalHours(double maxHours) { m_maxVerticalHours = maxHours; }

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    void setupUi();
    
    QVBoxLayout* m_mainLayout;
    QLabel* m_titleLabel;
    QLabel* m_summaryLabel;
    QScrollArea* m_scrollArea;
    QWidget* m_verticalBarsContainer;
    QHBoxLayout* m_verticalBarsLayout;
    
    double m_maxVerticalHours;
};

#endif // STATISTICSTIMELYVIEWWIDGET_H