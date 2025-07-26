#include "calendar/components/calendarnavigation.h"
#include "calendar/components/calendarstyles.h"
#include "calendar/components/calendarutils.h"
#include <QHBoxLayout>
#include <QPushButton>
#include <QMenu>
#include <QAction>
#include <QListWidget>
#include <QWidgetAction>

CalendarNavigation::CalendarNavigation(QWidget* parent)
    : QWidget(parent)
    , m_layout(nullptr)
    , m_previousButton(nullptr)
    , m_nextButton(nullptr)
    , m_todayButton(nullptr)
    , m_monthButton(nullptr)
    , m_yearButton(nullptr)
{
    setupUi();
}

void CalendarNavigation::setupUi()
{
    setStyleSheet(CalendarStyles::NAVIGATION_GRADIENT_STYLE);
    
    createButtons();
    setupLayout();
    connectSignals();
}

void CalendarNavigation::createButtons()
{
    // Previous month button
    m_previousButton = new QPushButton("◀", this);
    m_previousButton->setFixedSize(35, 35);
    m_previousButton->setStyleSheet(CalendarStyles::ROUND_BUTTON_STYLE);

    // Next month button
    m_nextButton = new QPushButton("▶", this);
    m_nextButton->setFixedSize(35, 35);
    m_nextButton->setStyleSheet(CalendarStyles::ROUND_BUTTON_STYLE);

    // Month button
    m_monthButton = new QPushButton("January", this);
    m_monthButton->setMinimumWidth(100);
    m_monthButton->setStyleSheet(CalendarStyles::MONTH_YEAR_BUTTON_STYLE);

    // Year button
    m_yearButton = new QPushButton("2024", this);
    m_yearButton->setMinimumWidth(70);
    m_yearButton->setStyleSheet(CalendarStyles::MONTH_YEAR_BUTTON_STYLE);

    // Today button
    m_todayButton = new QPushButton("Today", this);
    m_todayButton->setFixedHeight(35);
    m_todayButton->setStyleSheet(CalendarStyles::TODAY_BUTTON_STYLE);
}

void CalendarNavigation::setupLayout()
{
    m_layout = new QHBoxLayout(this);
    m_layout->setContentsMargins(10, 10, 10, 10);
    m_layout->setSpacing(10);

    m_layout->addWidget(m_previousButton);
    m_layout->addStretch();
    m_layout->addWidget(m_monthButton);
    m_layout->addSpacing(5);
    m_layout->addWidget(m_yearButton);
    m_layout->addStretch();
    m_layout->addWidget(m_nextButton);
    m_layout->addSpacing(15);
    m_layout->addWidget(m_todayButton);
}

void CalendarNavigation::connectSignals()
{
    connect(m_previousButton, &QPushButton::clicked, this, &CalendarNavigation::previousMonthClicked);
    connect(m_nextButton, &QPushButton::clicked, this, &CalendarNavigation::nextMonthClicked);
    connect(m_todayButton, &QPushButton::clicked, this, &CalendarNavigation::todayClicked);
    connect(m_monthButton, &QPushButton::clicked, this, &CalendarNavigation::onMonthButtonClicked);
    connect(m_yearButton, &QPushButton::clicked, this, &CalendarNavigation::onYearButtonClicked);
}

void CalendarNavigation::updateDisplay(const QDate& date)
{
    m_monthButton->setText(date.toString("MMMM"));
    m_yearButton->setText(date.toString("yyyy"));
}

void CalendarNavigation::onMonthButtonClicked()
{
    QMenu* monthMenu = new QMenu(this);
    monthMenu->setStyleSheet(CalendarStyles::getMenuStyle());
    monthMenu->setFixedWidth(m_monthButton->width());

    QStringList months = CalendarUtils::getMonthNames();
    for (int i = 0; i < months.size(); ++i) {
        QAction* action = monthMenu->addAction(months[i]);
        action->setData(i + 1);
        connect(action, &QAction::triggered, this, [this, i]() {
            emit monthSelected(i + 1);
        });
    }

    QPoint pos = m_monthButton->mapToGlobal(QPoint(0, m_monthButton->height()));
    monthMenu->exec(pos);
    monthMenu->deleteLater();
}

void CalendarNavigation::onYearButtonClicked()
{
    QMenu* yearMenu = new QMenu(this);
    yearMenu->setStyleSheet(CalendarStyles::getMenuStyle());
    yearMenu->setFixedWidth(m_yearButton->width());

    QListWidget* yearList = new QListWidget();
    yearList->setStyleSheet(CalendarStyles::getListStyle());
    yearList->setFixedSize(m_yearButton->width(), 150);

    QList<int> years = CalendarUtils::getYearRange();
    for (int year : years) {
        QListWidgetItem* item = new QListWidgetItem(QString::number(year));
        item->setData(Qt::UserRole, year);
        yearList->addItem(item);
    }

    QWidgetAction* widgetAction = new QWidgetAction(yearMenu);
    widgetAction->setDefaultWidget(yearList);
    yearMenu->addAction(widgetAction);

    connect(yearList, &QListWidget::itemClicked, this, [this, yearMenu](QListWidgetItem* item) {
        int year = item->data(Qt::UserRole).toInt();
        emit yearSelected(year);
        yearMenu->close();
    });

    QPoint pos = m_yearButton->mapToGlobal(QPoint(0, m_yearButton->height()));
    yearMenu->exec(pos);
    yearMenu->deleteLater();
}