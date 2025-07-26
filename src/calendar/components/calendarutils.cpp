#include "calendar/components/calendarutils.h"
#include <QDate>

QString CalendarUtils::formatDuration(int seconds)
{
    int hours = seconds / 3600;
    int minutes = (seconds % 3600) / 60;

    if (hours > 0) {
        return QString("%1h %2m").arg(hours).arg(minutes);
    } else {
        return QString("%1m").arg(minutes);
    }
}

QString CalendarUtils::formatDate(const QDate& date)
{
    return date.toString("dddd, MMMM d, yyyy");
}

QStringList CalendarUtils::getMonthNames()
{
    return {"January", "February", "March", "April", "May", "June",
            "July", "August", "September", "October", "November", "December"};
}

QList<int> CalendarUtils::getYearRange(int currentYear, int range)
{
    QList<int> years;
    for (int year = currentYear; year <= currentYear + range; ++year) {
        years.append(year);
    }
    return years;
}