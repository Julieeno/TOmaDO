#include "statistics/components/statisticsdatamanager.h"
#include <QDebug>
#include <QMap>
#include <QStringList>
#include <QApplication>
#include <QDateTime>
#include <QSet>
#include <cmath>

StatisticsDataManager::StatisticsDataManager(QObject* parent)
    : QObject(parent)
    , m_maxHours(0.0)
    , m_maxVerticalHours(0.0)
    , m_refreshTimer(new QTimer(this))
{
    m_refreshTimer->setSingleShot(true);
    m_refreshTimer->setInterval(100);
    connect(m_refreshTimer, &QTimer::timeout, this, &StatisticsDataManager::dataRefreshed);
}

void StatisticsDataManager::refreshFromDatabase()
{
    // Get all timer records from the beginning of time to now
    QDateTime start = QDateTime::fromMSecsSinceEpoch(0);
    QDateTime end = QDateTime::currentDateTime();

    m_timerRecords = DatabaseManager::instance().getTimerRecords(start, end);
    m_goals = DatabaseManager::instance().getAllGoals(true);
    m_refreshTimer->start();
}

void StatisticsDataManager::loadTimeBasedStatistics(const QString& rangeType, const QDate& referenceDate)
{
    QDate startDate = getPeriodStartDate(rangeType, referenceDate);
    QDate endDate = getPeriodEndDate(rangeType, referenceDate);

    QStringList expectedGroups = getExpectedTimeGroups(rangeType, startDate, endDate);
    QMap<QString, QList<StatisticsVerticalBarWidget::Segment>> groupedData;

    // Initialize all expected groups
    for (const QString& group : expectedGroups) {
        groupedData[group] = QList<StatisticsVerticalBarWidget::Segment>();
    }

    // Group timer records by time periods
    for (const auto& record : m_timerRecords) {
        QDate recordDate = record.startTime.date();
        QTime recordTime = record.startTime.time();

        if (recordDate >= startDate && recordDate <= endDate) {
            QString groupLabel = getTimeGroupLabel(recordDate, recordTime, rangeType, referenceDate);

            // Find the corresponding goal
            QString goalTitle = "No Goal";
            QColor goalColor = QColor("#C0C0C0");
            int goalId = -1;

            for (const auto& goal : m_goals) {
                if (goal.id == record.goalId) {
                    goalTitle = goal.title;
                    goalColor = QColor(goal.colorCode.isEmpty() ? "#FF6B6B" : goal.colorCode);
                    goalId = goal.id;
                    break;
                }
            }

            StatisticsVerticalBarWidget::Segment segment;
            segment.label = groupLabel;
            segment.goalTitle = goalTitle;
            segment.hours = record.startTime.secsTo(record.endTime) / 3600.0; // Convert seconds to hours
            segment.color = goalColor;
            segment.goalId = goalId;

            groupedData[groupLabel].append(segment);
        }
    }

    // Consolidate segments by goal within each group
    for (auto it = groupedData.begin(); it != groupedData.end(); ++it) {
        QMap<int, StatisticsVerticalBarWidget::Segment> consolidatedSegments;

        for (const auto& segment : it.value()) {
            if (consolidatedSegments.contains(segment.goalId)) {
                consolidatedSegments[segment.goalId].hours += segment.hours;
            } else {
                consolidatedSegments[segment.goalId] = segment;
            }
        }

        it.value().clear();
        for (const auto& segment : consolidatedSegments) {
            it.value().append(segment);
        }
    }

    // Calculate maximum hours for scaling
    m_maxVerticalHours = 0.0;
    for (auto it = groupedData.begin(); it != groupedData.end(); ++it) {
        double totalHours = 0.0;
        for (const auto& segment : it.value()) {
            totalHours += segment.hours;
        }
        m_maxVerticalHours = std::max(m_maxVerticalHours, totalHours);
    }

    if (m_maxVerticalHours == 0.0) {
        m_maxVerticalHours = 1.0;
    }

    // Convert to output format
    QList<QPair<QString, QList<StatisticsVerticalBarWidget::Segment>>> outputData;
    for (const QString& group : expectedGroups) {
        outputData.append(qMakePair(group, groupedData[group]));
    }

    // Generate summary
    double totalHours = 0.0;
    int totalSessions = 0;
    for (const auto& record : m_timerRecords) {
        QDate recordDate = record.startTime.date();
        if (recordDate >= startDate && recordDate <= endDate) {
            totalHours += record.startTime.secsTo(record.endTime) / 3600.0;
            totalSessions++;
        }
    }

    QString summaryText = QString("Total: %1 in %2 sessions")
                         .arg(formatHours(totalHours))
                         .arg(totalSessions);

    emit timeBasedDataLoaded(outputData, summaryText);
}

void StatisticsDataManager::loadGoalStatistics(const QString& rangeType, const QDate& referenceDate)
{
    QDate startDate = getPeriodStartDate(rangeType, referenceDate);
    QDate endDate = getPeriodEndDate(rangeType, referenceDate);

    QMap<int, double> goalHours;

    // Calculate hours per goal (including -1 for "No Goal")
    for (const auto& record : m_timerRecords) {
        QDate recordDate = record.startTime.date();
        if (recordDate >= startDate && recordDate <= endDate) {
            goalHours[record.goalId] += record.startTime.secsTo(record.endTime) / 3600.0;
        }
    }

    // Find maximum hours for scaling
    m_maxHours = 0.0;
    for (auto it = goalHours.begin(); it != goalHours.end(); ++it) {
        m_maxHours = std::max(m_maxHours, it.value());
    }

    if (m_maxHours == 0.0) {
        m_maxHours = 1.0;
    }

    // Convert to output format
    QList<QPair<QString, QPair<double, QColor>>> outputData;
    double totalHours = 0.0;
    int activeGoals = 0;

    // First, add all goals that have recorded time
    for (const auto& goal : m_goals) {
        if (goalHours.contains(goal.id)) {
            double hours = goalHours[goal.id];
            QColor goalColor = QColor(goal.colorCode.isEmpty() ? "#FF6B6B" : goal.colorCode);
            outputData.append(qMakePair(goal.title, qMakePair(hours, goalColor)));
            totalHours += hours;
            activeGoals++;
        }
    }

    if (goalHours.contains(-1)) {
        double hours = goalHours[-1];
        QColor noGoalColor = QColor("#C0C0C0");
        outputData.append(qMakePair(QString("No Goal"), qMakePair(hours, noGoalColor)));
        totalHours += hours;
        activeGoals++;
    }

    for (auto it = goalHours.begin(); it != goalHours.end(); ++it) {
        int goalId = it.key();
        if (goalId != -1) {
            bool foundInGoals = false;
            for (const auto& goal : m_goals) {
                if (goal.id == goalId) {
                    foundInGoals = true;
                    break;
                }
            }

            if (!foundInGoals) {
                double hours = it.value();
                QColor unknownGoalColor = QColor("#999999");
                outputData.append(qMakePair(QString("No Goal (%1)").arg(goalId), qMakePair(hours, unknownGoalColor)));
                totalHours += hours;
                activeGoals++;
            }
        }
    }

    // Sort by hours (descending)
    std::sort(outputData.begin(), outputData.end(),
              [](const QPair<QString, QPair<double, QColor>>& a, const QPair<QString, QPair<double, QColor>>& b) {
                  return a.second.first > b.second.first;
              });

    // Generate summary
    QString summaryText = QString("Total: %1 across %2 goals")
                         .arg(formatHours(totalHours))
                         .arg(activeGoals);

    emit goalDataLoaded(outputData, summaryText);
}

QString StatisticsDataManager::formatHours(double hours) const
{
    if (hours == 0) {
        return "0h";
    }
    if (hours < 1) {
        return QString("%1m").arg(qRound(hours * 60));
    }
    return QString("%1h %2m").arg(floor(hours)).arg(qRound((hours - floor(hours)) * 60));
}

QString StatisticsDataManager::getTimeGroupLabel(const QDate& recordDate, const QString& rangeType, const QDate& referenceDate) const
{
    if (rangeType == "daily") {
        return recordDate.toString("MMM d");
    } else if (rangeType == "weekly") {
        return recordDate.toString("ddd");
    } else if (rangeType == "monthly") {
        // For monthly view, group by weeks
        int weekOfYear = recordDate.weekNumber();
        return QString("Week %1").arg(weekOfYear);
    }
    return recordDate.toString("MMM d");
}

QString StatisticsDataManager::getTimeGroupLabel(const QDate& recordDate, const QTime& recordTime, const QString& rangeType, const QDate& referenceDate) const
{
    if (rangeType == "daily") {
        // For daily view, group by 3-hour periods
        int hour = recordTime.hour();
        int periodStart = (hour / 3) * 3;
        int periodEnd = periodStart + 3;

        // Handle the last period of the day
        if (periodEnd > 24) {
            periodEnd = 24;
        }

        return QString("%1-%2").arg(periodStart).arg(periodEnd);
    } else if (rangeType == "weekly") {
        // For weekly view, group by days of the week
        return recordDate.toString("ddd");
    } else if (rangeType == "monthly") {
        // For monthly view, group by weeks
        int weekOfYear = recordDate.weekNumber();
        return QString("Week %1").arg(weekOfYear);
    }

    return recordDate.toString("MMM d");
}

QColor StatisticsDataManager::getColorForTimeGroup(const QString& label, const QString& rangeType, const QDate& referenceDate) const
{
    QStringList colors = {"#FF6B6B", "#4ECDC4", "#45B7D1", "#96CEB4", "#FFEAA7", "#DDA0DD", "#98D8C8", "#F7DC6F"};
    uint hash = qHash(label);
    return QColor(colors[hash % colors.size()]);
}

QDate StatisticsDataManager::getPeriodStartDate(const QString& rangeType, const QDate& referenceDate) const
{
    if (rangeType == "daily") {
        return referenceDate;
    } else if (rangeType == "weekly") {
        return referenceDate.addDays(-(referenceDate.dayOfWeek() - 1));
    } else if (rangeType == "monthly") {
        return QDate(referenceDate.year(), referenceDate.month(), 1);
    }
    return referenceDate;
}

QDate StatisticsDataManager::getPeriodEndDate(const QString& rangeType, const QDate& referenceDate) const
{
    if (rangeType == "daily") {
        return referenceDate;
    } else if (rangeType == "weekly") {
        return getPeriodStartDate(rangeType, referenceDate).addDays(6);
    } else if (rangeType == "monthly") {
        return QDate(referenceDate.year(), referenceDate.month(), referenceDate.daysInMonth());
    }
    return referenceDate;
}

QStringList StatisticsDataManager::getExpectedTimeGroups(const QString& rangeType, const QDate& startDate, const QDate& endDate) const
{
    QStringList groups;

    if (rangeType == "daily") {
        // For daily view, show 3-hour periods: 0-3, 3-6, 6-9, 9-12, 12-15, 15-18, 18-21, 21-24
        for (int hour = 0; hour < 24; hour += 3) {
            int periodEnd = hour + 3;
            if (periodEnd > 24) {
                periodEnd = 24;
            }
            groups.append(QString("%1-%2").arg(hour).arg(periodEnd));
        }
    } else if (rangeType == "weekly") {
        // For weekly view, show days of the week: Monday, Tuesday, Wednesday, Thursday, Friday, Saturday, Sunday
        QDate date = startDate;
        while (date <= endDate) {
            groups.append(date.toString("ddd"));
            date = date.addDays(1);
        }
    } else if (rangeType == "monthly") {
        // For monthly view, show weeks: Week 1, Week 2, etc.
        QSet<int> weekNumbers;
        QDate date = startDate;
        while (date <= endDate) {
            weekNumbers.insert(date.weekNumber());
            date = date.addDays(1);
        }

        QList<int> sortedWeeks(weekNumbers.begin(), weekNumbers.end());
        std::sort(sortedWeeks.begin(), sortedWeeks.end());

        for (int week : sortedWeeks) {
            groups.append(QString("Week %1").arg(week));
        }
    }
    
    return groups;
}