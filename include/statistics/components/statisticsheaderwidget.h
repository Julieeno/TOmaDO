#ifndef STATISTICSHEADERWIDGET_H
#define STATISTICSHEADERWIDGET_H

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMenu>
#include <QActionGroup>
#include <QDate>

class StatisticsHeaderWidget : public QWidget {
    Q_OBJECT

public:
    explicit StatisticsHeaderWidget(QWidget* parent = nullptr);

    void setCurrentTimeRange(const QString& range);
    void setCurrentReferenceDate(const QDate& date);
    void updateNavigationButtons();
    void updatePeriodLabel();

    QString getCurrentTimeRange() const { return m_currentTimeRange; }
    QDate getCurrentReferenceDate() const { return m_currentReferenceDate; }

    signals:
        void timeRangeChanged();
    void refreshClicked();
    void previousPeriod();
    void nextPeriod();
    void todayClicked();

protected:
    void paintEvent(QPaintEvent* event) override;

private slots:
    void onTimeRangeMenuTriggered();

private:
    void setupUi();
    void setupTimeRangeMenu();
    void connectSignals();
    QString getPeriodDisplayName(const QString& rangeType, const QDate& referenceDate) const;
    QDate getPeriodStartDate(const QString& rangeType, const QDate& referenceDate) const;
    QDate getPeriodEndDate(const QString& rangeType, const QDate& referenceDate) const;

    QVBoxLayout* m_mainLayout;
    QHBoxLayout* m_headerLayout;
    QPushButton* m_timeRangeButton;
    QMenu* m_timeRangeMenu;
    QActionGroup* m_timeRangeActionGroup;
    QPushButton* m_refreshButton;
    QPushButton* m_previousButton;
    QPushButton* m_nextButton;
    QPushButton* m_todayButton;
    QLabel* m_titleLabel;
    QLabel* m_periodLabel;

    QString m_currentTimeRange;
    QDate m_currentReferenceDate;
};

#endif // STATISTICSHEADERWIDGET_H