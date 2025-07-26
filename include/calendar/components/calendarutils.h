#ifndef CALENDARUTILS_H
#define CALENDARUTILS_H

#include <QString>
#include <QDate>

class CalendarUtils {
public:
    static QString formatDuration(int seconds);
    static QString formatDate(const QDate& date);
    static QStringList getMonthNames();
    static QList<int> getYearRange(int currentYear = 2025, int range = 20);

private:
    CalendarUtils() = default;
};

#endif // CALENDARUTILS_H