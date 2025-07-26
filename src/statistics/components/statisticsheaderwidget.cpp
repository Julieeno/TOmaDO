#include "statistics/components/statisticsheaderwidget.h"
#include <QAction>
#include <QApplication>
#include <QPainter>
#include <QPainterPath>
#include <QLinearGradient>

StatisticsHeaderWidget::StatisticsHeaderWidget(QWidget* parent)
    : QWidget(parent)
    , m_mainLayout(nullptr)
    , m_headerLayout(nullptr)
    , m_timeRangeButton(nullptr)
    , m_timeRangeMenu(nullptr)
    , m_timeRangeActionGroup(nullptr)
    , m_refreshButton(nullptr)
    , m_previousButton(nullptr)
    , m_nextButton(nullptr)
    , m_todayButton(nullptr)
    , m_titleLabel(nullptr)
    , m_periodLabel(nullptr)
    , m_currentTimeRange("daily")
    , m_currentReferenceDate(QDate::currentDate())
{
    QLocale::setDefault(QLocale(QLocale::English));
    setupUi();
    setupTimeRangeMenu();
    connectSignals();
}

void StatisticsHeaderWidget::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Create rounded rectangle path
    QPainterPath path;
    path.addRoundedRect(rect(), 15, 15);

    // Create gradient
    QLinearGradient gradient(0, 0, 0, height());
    gradient.setColorAt(0, QColor(255, 245, 245)); // #FFF5F5
    gradient.setColorAt(1, QColor(255, 232, 232)); // #FFE8E8

    // Fill with gradient
    painter.fillPath(path, gradient);

    // Draw border
    painter.setPen(QPen(QColor(255, 208, 208), 1)); // #FFD0D0
    painter.drawPath(path);

    QWidget::paintEvent(event);
}

void StatisticsHeaderWidget::setupUi()
{
    // Set transparent background so our custom paint shows through
    setAutoFillBackground(false);

    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setSpacing(0);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);

    // Create header container with proper layout
    QWidget* headerContainer = new QWidget();
    headerContainer->setFixedHeight(80);
    headerContainer->setStyleSheet("QWidget { background-color: transparent; }");

    QHBoxLayout* headerLayout = new QHBoxLayout(headerContainer);
    headerLayout->setSpacing(0);
    headerLayout->setContentsMargins(20, 0, 20, 0);

    // Left section - Title
    QWidget* leftSection = new QWidget();
    leftSection->setFixedWidth(550);
    leftSection->setStyleSheet("QWidget { background-color: transparent; }");
    QHBoxLayout* leftLayout = new QHBoxLayout(leftSection);
    leftLayout->setContentsMargins(0, 0, 0, 0);

    m_titleLabel = new QLabel("Overview Statistics");
    QFont titleFont = m_titleLabel->font();
    titleFont.setPointSize(24);
    titleFont.setBold(true);
    m_titleLabel->setFont(titleFont);
    m_titleLabel->setStyleSheet("QLabel { color: #A64545; background-color: transparent; }");
    leftLayout->addWidget(m_titleLabel);
    leftLayout->addStretch();

    headerLayout->addWidget(leftSection);

    // Center section - Navigation buttons
    QWidget* centerSection = new QWidget();
    centerSection->setFixedWidth(400);
    centerSection->setStyleSheet("QWidget { background-color: transparent; }");
    QHBoxLayout* centerLayout = new QHBoxLayout(centerSection);
    centerLayout->setContentsMargins(0, 0, 0, 0);
    centerLayout->setSpacing(8);

    centerLayout->addStretch();

    // Previous button
    m_previousButton = new QPushButton("◀");
    m_previousButton->setFixedSize(36, 36);
    m_previousButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #FF9999;"
        "   border: 2px solid #ffb3b3;"
        "   border-radius: 18px;"
        "   color: white;"
        "   font-weight: bold;"
        "   font-size: 16px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #FF7777;"
        "   color: #7a4d4d;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #FF5555;"
        "}"
    );
    centerLayout->addWidget(m_previousButton);

    // Today button
    m_todayButton = new QPushButton("Today");
    m_todayButton->setFixedHeight(36);
    m_todayButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #FFAAAA;"
        "   border: 2px solid #ffb3b3;"
        "   border-radius: 12px;"
        "   color: #8B3A3A;"
        "   font-weight: 600;"
        "   padding: 8px 16px;"
        "   font-size: 14px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #FF8888;"
        "   border-color: #ff9999;"
        "   color: white;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #FF6666;"
        "   border-color: #ff8080;"
        "}"
    );
    centerLayout->addWidget(m_todayButton);

    // Time range selector
    m_timeRangeButton = new QPushButton("Daily");
    m_timeRangeButton->setFixedHeight(40);
    m_timeRangeButton->setStyleSheet(
        "QPushButton {"
        "   background-color: rgba(255, 255, 255, 0.8);"
        "   border: 2px solid #FFD0D0;"
        "   border-radius: 12px;"
        "   color: #8B3A3A;"
        "   font-weight: bold;"
        "   padding: 8px 16px;"
        "   font-size: 14px;"
        "}"
        "QPushButton:hover {"
        "   background-color: rgba(255, 255, 255, 1.0);"
        "   border: 2px solid #FF9999;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #FFB3B3;"
        "}"
        "QPushButton::menu-indicator {"
        "   width: 0px;"
        "   height: 0px;"
        "}"
    );
    centerLayout->addWidget(m_timeRangeButton);

    // Next button
    m_nextButton = new QPushButton("▶");
    m_nextButton->setFixedSize(36, 36);
    m_nextButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #FF9999;"
        "   border: 2px solid #ffb3b3;"
        "   border-radius: 18px;"
        "   color: white;"
        "   font-weight: bold;"
        "   font-size: 16px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #FF7777;"
        "   color: #7a4d4d;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #FF5555;"
        "}"
    );
    centerLayout->addWidget(m_nextButton);

    centerLayout->addStretch();
    headerLayout->addWidget(centerSection);

    // Right section - Period label and refresh button
    QWidget* rightSection = new QWidget();
    rightSection->setStyleSheet("QWidget { background-color: transparent; }");
    QHBoxLayout* rightLayout = new QHBoxLayout(rightSection);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(15);

    rightLayout->addStretch();

    // Selected date label
    m_periodLabel = new QLabel();
    QFont periodFont = m_periodLabel->font();
    periodFont.setPointSize(14);
    periodFont.setBold(true);
    m_periodLabel->setFont(periodFont);
    m_periodLabel->setStyleSheet("QLabel { color: #8b5a5a; background-color: transparent; }");
    m_periodLabel->setMinimumWidth(200);
    m_periodLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    rightLayout->addWidget(m_periodLabel);

    // Refresh button
    m_refreshButton = new QPushButton("Refresh");
    m_refreshButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #FFAAAA;"
        "   border: 2px solid #ffb3b3;"
        "   border-radius: 12px;"
        "   color: #8B3A3A;"
        "   font-weight: 600;"
        "   padding: 8px 16px;"
        "   font-size: 14px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #FF8888;"
        "   border-color: #ff9999;"
        "   color: white;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #FF6666;"
        "   border-color: #ff8080;"
        "}"
    );
    rightLayout->addWidget(m_refreshButton);

    headerLayout->addWidget(rightSection);

    m_mainLayout->addWidget(headerContainer);
    updatePeriodLabel();
}

void StatisticsHeaderWidget::setupTimeRangeMenu()
{
    m_timeRangeMenu = new QMenu(this);
    m_timeRangeActionGroup = new QActionGroup(this);

    QAction* dailyAction = new QAction("Daily", this);
    dailyAction->setCheckable(true);
    dailyAction->setChecked(true);
    dailyAction->setData("daily");

    QAction* weeklyAction = new QAction("Weekly", this);
    weeklyAction->setCheckable(true);
    weeklyAction->setData("weekly");

    QAction* monthlyAction = new QAction("Monthly", this);
    monthlyAction->setCheckable(true);
    monthlyAction->setData("monthly");

    m_timeRangeActionGroup->addAction(dailyAction);
    m_timeRangeActionGroup->addAction(weeklyAction);
    m_timeRangeActionGroup->addAction(monthlyAction);

    m_timeRangeMenu->addAction(dailyAction);
    m_timeRangeMenu->addAction(weeklyAction);
    m_timeRangeMenu->addAction(monthlyAction);

    m_timeRangeMenu->setStyleSheet(
        "QMenu {"
        "   background-color: white;"
        "   border: none;"
        "   border-radius: 8px;"
        "   padding: 4px;"
        "   color: #8B3A3A;"
        "   font-weight: bold;"
        "}"
        "QMenu::item {"
        "   padding: 8px 12px;"
        "   border-radius: 4px;"
        "   margin: 1px;"
        "}"
        "QMenu::item:selected {"
        "   background-color: #FFB3B3;"
        "}"
    );

    m_timeRangeButton->setMenu(m_timeRangeMenu);
}

void StatisticsHeaderWidget::connectSignals()
{
    connect(m_timeRangeActionGroup, &QActionGroup::triggered, this, &StatisticsHeaderWidget::onTimeRangeMenuTriggered);
    connect(m_refreshButton, &QPushButton::clicked, this, &StatisticsHeaderWidget::refreshClicked);
    connect(m_previousButton, &QPushButton::clicked, this, &StatisticsHeaderWidget::previousPeriod);
    connect(m_nextButton, &QPushButton::clicked, this, &StatisticsHeaderWidget::nextPeriod);
    connect(m_todayButton, &QPushButton::clicked, this, &StatisticsHeaderWidget::todayClicked);
}

void StatisticsHeaderWidget::onTimeRangeMenuTriggered()
{
    QAction* action = m_timeRangeActionGroup->checkedAction();
    if (action) {
        m_currentTimeRange = action->data().toString();
        m_timeRangeButton->setText(action->text());
        updatePeriodLabel();
        emit timeRangeChanged();
    }
}

void StatisticsHeaderWidget::setCurrentTimeRange(const QString& range)
{
    m_currentTimeRange = range;
    QString buttonText = range;
    buttonText[0] = buttonText[0].toUpper();
    m_timeRangeButton->setText(buttonText);
    updatePeriodLabel();
}

void StatisticsHeaderWidget::setCurrentReferenceDate(const QDate& date)
{
    m_currentReferenceDate = date;
    updatePeriodLabel();
}

void StatisticsHeaderWidget::updateNavigationButtons()
{
    QDate today = QDate::currentDate();
    QDate periodStart = getPeriodStartDate(m_currentTimeRange, today);
    QDate currentPeriodStart = getPeriodStartDate(m_currentTimeRange, m_currentReferenceDate);

    bool isCurrentPeriod = (currentPeriodStart == periodStart);
    m_nextButton->setEnabled(!isCurrentPeriod);
    m_todayButton->setEnabled(!isCurrentPeriod);
}

void StatisticsHeaderWidget::updatePeriodLabel()
{
    QLocale locale(QLocale::English);

    QString periodText = getPeriodDisplayName(m_currentTimeRange, m_currentReferenceDate);
    m_periodLabel->setText(periodText);
}

QString StatisticsHeaderWidget::getPeriodDisplayName(const QString& rangeType, const QDate& referenceDate) const
{
    QLocale locale(QLocale::English);

    if (rangeType == "daily") {
        return locale.toString(referenceDate, "MMMM d, yyyy");
    } else if (rangeType == "weekly") {
        QDate weekStart = getPeriodStartDate(rangeType, referenceDate);
        QDate weekEnd = getPeriodEndDate(rangeType, referenceDate);
        return locale.toString(weekStart, "MMM d") + " - " +
               locale.toString(weekEnd, "MMM d, yyyy");
    } else if (rangeType == "monthly") {
        return locale.toString(referenceDate, "MMMM yyyy");
    }

    return "";
}

QDate StatisticsHeaderWidget::getPeriodStartDate(const QString& rangeType, const QDate& referenceDate) const
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

QDate StatisticsHeaderWidget::getPeriodEndDate(const QString& rangeType, const QDate& referenceDate) const
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