#ifndef CALENDARSTYLES_H
#define CALENDARSTYLES_H

#include <QString>

namespace CalendarStyles {
    // Color constants
    namespace Colors {
        constexpr const char* BACKGROUND = "#FEFBFB";
        constexpr const char* TEXT_PRIMARY = "#2C1810";
        constexpr const char* TEXT_SECONDARY = "#8B3A3A";
        constexpr const char* TEXT_ACCENT = "#A64545";
        constexpr const char* BORDER_LIGHT = "#FFD0D0";
        constexpr const char* BORDER_MEDIUM = "#FFB3B3";
        constexpr const char* BUTTON_PRIMARY = "#FF9999";
        constexpr const char* BUTTON_HOVER = "#FF7777";
        constexpr const char* BUTTON_PRESSED = "#FF5555";
        constexpr const char* BUTTON_SECONDARY = "#FFAAAA";
        constexpr const char* GRADIENT_START = "#FFF5F5";
        constexpr const char* GRADIENT_END = "#FFE8E8";
        constexpr const char* SELECTION_BG = "#FFB3B3";
        constexpr const char* HEADER_BG = "#FFE0E0";
    }

    // Style strings
    const QString MAIN_WIDGET_STYLE = QString(
        "QWidget { background-color: %1; color: %2; }"
    ).arg(Colors::BACKGROUND, Colors::TEXT_PRIMARY);

    const QString HEADER_GRADIENT_STYLE = QString(
        "QWidget {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 %1, stop:1 %2);"
        "   border-radius: 15px;"
        "   border: none;"
        "}"
    ).arg(Colors::GRADIENT_START, Colors::GRADIENT_END);

    const QString NAVIGATION_GRADIENT_STYLE = QString(
        "QWidget {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #FFF0F0, stop:1 %1);"
        "   border-radius: 8px;"
        "   padding: 10px;"
        "}"
    ).arg(Colors::GRADIENT_END);

    const QString ROUND_BUTTON_STYLE = QString(
        "QPushButton {"
        "   background-color: %1;"
        "   border: none;"
        "   border-radius: 17px;"
        "   color: white;"
        "   font-size: 14px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover { background-color: %2; }"
        "QPushButton:pressed { background-color: %3; }"
    ).arg(Colors::BUTTON_PRIMARY, Colors::BUTTON_HOVER, Colors::BUTTON_PRESSED);

    const QString MONTH_YEAR_BUTTON_STYLE = QString(
        "QPushButton {"
        "   background-color: rgba(255, 255, 255, 0.8);"
        "   border: 2px solid %1;"
        "   border-radius: 6px;"
        "   color: %2;"
        "   font-size: 14px;"
        "   font-weight: bold;"
        "   padding: 6px 12px;"
        "}"
        "QPushButton:hover {"
        "   background-color: rgba(255, 255, 255, 1.0);"
        "   border: 2px solid %3;"
        "}"
        "QPushButton:pressed { background-color: %4; }"
    ).arg(Colors::BORDER_LIGHT, Colors::TEXT_SECONDARY, Colors::BUTTON_PRIMARY, Colors::BORDER_MEDIUM);

    const QString TODAY_BUTTON_STYLE = QString(
        "QPushButton {"
        "   background-color: %1;"
        "   border: 2px solid #FF8888;"
        "   border-radius: 17px;"
        "   color: %2;"
        "   font-size: 11px;"
        "   font-weight: bold;"
        "   padding: 6px 12px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #FF8888;"
        "   color: white;"
        "}"
        "QPushButton:pressed { background-color: #FF6666; }"
    ).arg(Colors::BUTTON_SECONDARY, Colors::TEXT_SECONDARY);

    QString getCalendarStyle();
    QString getDetailsStyle();
    QString getListStyle();
    QString getMenuStyle();
}

#endif // CALENDARSTYLES_H