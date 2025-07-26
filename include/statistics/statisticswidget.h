#ifndef STATISTICSWIDGET_H
#define STATISTICSWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMenu>
#include <QActionGroup>
#include <QScrollArea>
#include <QFrame>
#include <QDate>
#include <QMap>
#include <QList>
#include <QTimer>

#include "database/databasemanager.h"

// Forward declarations
class StatisticsHeaderWidget;
class StatisticsTimelyViewWidget;
class StatisticsGoalsViewWidget;
class StatisticsDataManager;

class StatisticsBarWidget : public QWidget {
    Q_OBJECT

public:
    explicit StatisticsBarWidget(const QString& label, double hours, double maxHours,
                                const QColor& color = QColor("#FF6B6B"), QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    static const int BAR_HEIGHT = 50;
    static const int LABEL_WIDTH = 160;
    static const int MARGIN = 8;

    QString m_label;
    double m_hours;
    double m_maxHours;
    QColor m_barColor;
};

class StatisticsVerticalBarWidget : public QWidget {
    Q_OBJECT

public:
    struct Segment {
        QString label;
        QString goalTitle;
        double hours;
        QColor color;
        int goalId;
    };

    explicit StatisticsVerticalBarWidget(const QString& dateLabel, double maxHours,
                                       const QList<Segment>& segments,
                                       QWidget* parent = nullptr);

    QSize sizeHint() const override;
    static const int BAR_WIDTH = 90;

protected:
    void paintEvent(QPaintEvent* event) override;
    bool event(QEvent* event) override;

private:
    int getSegmentIndexAtPosition(const QPoint& pos) const;

    static const int LABEL_HEIGHT = 60;
    static const int MARGIN = 8;

    QString m_dateLabel;
    QList<Segment> m_segments;
    double m_maxHours;
};

class StatisticsWidget : public QWidget {
    Q_OBJECT

public:
    explicit StatisticsWidget(QWidget* parent = nullptr);

public slots:
    void refreshFromDatabase();

private slots:
    void onTimeRangeChanged();
    void onRefreshClicked();
    void onPreviousPeriod();
    void onNextPeriod();
    void onTodayClicked();
    void onTimeBasedDataLoaded(const QList<QPair<QString, QList<StatisticsVerticalBarWidget::Segment>>>& data,
                              const QString& summaryText);
    void onGoalDataLoaded(const QList<QPair<QString, QPair<double, QColor>>>& data,
                         const QString& summaryText);

private:
    void setupUi();
    void connectSignals();
    void updateStatistics();

    QVBoxLayout* m_mainLayout;

    StatisticsHeaderWidget* m_headerWidget;
    StatisticsTimelyViewWidget* m_timelyViewWidget;
    StatisticsGoalsViewWidget* m_goalsViewWidget;
    StatisticsDataManager* m_dataManager;
};

#endif // STATISTICSWIDGET_H