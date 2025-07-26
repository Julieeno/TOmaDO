#ifndef STATISTICSDATAMANAGER_H
#define STATISTICSDATAMANAGER_H

#include <QObject>
#include <QDate>
#include <QTime>
#include <QColor>
#include <QList>
#include <QString>
#include <QStringList>
#include <QTimer>
#include "database/databasemanager.h"
#include "statistics/statisticswidget.h"

class StatisticsDataManager : public QObject {
    Q_OBJECT

public:
    explicit StatisticsDataManager(QObject* parent = nullptr);
    
    void loadTimeBasedStatistics(const QString& rangeType, const QDate& referenceDate);
    void loadGoalStatistics(const QString& rangeType, const QDate& referenceDate);
    void refreshFromDatabase();
    
    QString formatHours(double hours) const;
    double getMaxHours() const { return m_maxHours; }
    double getMaxVerticalHours() const { return m_maxVerticalHours; }

signals:
    void timeBasedDataLoaded(const QList<QPair<QString, QList<StatisticsVerticalBarWidget::Segment>>>& data, 
                            const QString& summaryText);
    void goalDataLoaded(const QList<QPair<QString, QPair<double, QColor>>>& data, 
                       const QString& summaryText);
    void dataRefreshed();

private:
    QString getTimeGroupLabel(const QDate& recordDate, const QString& rangeType,
                             const QDate& referenceDate) const;
    QString getTimeGroupLabel(const QDate& recordDate, const QTime& recordTime,
                             const QString& rangeType, const QDate& referenceDate) const;
    QColor getColorForTimeGroup(const QString& label, const QString& rangeType,
                               const QDate& referenceDate) const;
    QDate getPeriodStartDate(const QString& rangeType, const QDate& referenceDate) const;
    QDate getPeriodEndDate(const QString& rangeType, const QDate& referenceDate) const;
    QStringList getExpectedTimeGroups(const QString& rangeType, const QDate& startDate, 
                                     const QDate& endDate) const;

    QList<DatabaseManager::TimerRecord> m_timerRecords;
    QList<DatabaseManager::GoalItem> m_goals;
    double m_maxHours;
    double m_maxVerticalHours;
    
    QTimer* m_refreshTimer;
};

#endif // STATISTICSDATAMANAGER_H