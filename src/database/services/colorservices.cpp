#include "database/services/colorservice.h"
#include "database/databaseconstants.h"

ColorService& ColorService::instance()
{
    static ColorService instance;
    return instance;
}

QString ColorService::getColorName(const QString& colorCode) const
{
    return ColorConstants::COLOR_NAMES.value(colorCode, colorCode);
}

QStringList ColorService::getAvailableColors() const
{
    return ColorConstants::AVAILABLE_COLORS;
}

bool ColorService::isValidColor(const QString& colorCode) const
{
    return ColorConstants::AVAILABLE_COLORS.contains(colorCode);
}