#include "statistics/components/statisticsgoalsviewwidget.h"
#include "statistics/statisticswidget.h"
#include <QPainter>
#include <QPainterPath>
#include <QLinearGradient>

StatisticsGoalsViewWidget::StatisticsGoalsViewWidget(QWidget* parent)
    : QWidget(parent)
    , m_mainLayout(nullptr)
    , m_titleLabel(nullptr)
    , m_summaryLabel(nullptr)
    , m_scrollArea(nullptr)
    , m_barsContainer(nullptr)
    , m_barsLayout(nullptr)
    , m_maxHours(0.0)
{
    setupUi();
}

void StatisticsGoalsViewWidget::paintEvent(QPaintEvent* event)
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

void StatisticsGoalsViewWidget::setupUi()
{
    // Set transparent background so our custom paint shows through
    setAutoFillBackground(false);

    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setSpacing(15);
    m_mainLayout->setContentsMargins(20, 20, 20, 20);

    // Header layout with title and summary - transparent background
    QHBoxLayout* headerLayout = new QHBoxLayout();
    headerLayout->setSpacing(10);

    // Title
    m_titleLabel = new QLabel("Goals Breakdown", this);
    QFont titleFont = m_titleLabel->font();
    titleFont.setPointSize(18);
    titleFont.setWeight(QFont::Bold);
    m_titleLabel->setFont(titleFont);
    m_titleLabel->setStyleSheet("QLabel { color: #A64545; background-color: transparent; }");

    // Summary
    m_summaryLabel = new QLabel(this);
    QFont summaryFont = m_summaryLabel->font();
    summaryFont.setPointSize(12);
    summaryFont.setWeight(QFont::Medium);
    m_summaryLabel->setFont(summaryFont);
    m_summaryLabel->setStyleSheet("QLabel { color: #6c757d; background-color: transparent; }");
    m_summaryLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    headerLayout->addWidget(m_titleLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(m_summaryLabel);

    m_mainLayout->addLayout(headerLayout);

    // Scroll area for horizontal bars
    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setMinimumHeight(200);
    m_scrollArea->setStyleSheet(
        "QScrollArea {"
        "   border: none;"
        "   background-color: transparent;"
        "}"
    );

    m_barsContainer = new QWidget();
    m_barsContainer->setStyleSheet("QWidget { background-color: transparent; }");
    m_barsLayout = new QVBoxLayout(m_barsContainer);
    m_barsLayout->setSpacing(10);
    m_barsLayout->setContentsMargins(15, 15, 15, 15);
    m_barsLayout->setAlignment(Qt::AlignTop);

    m_scrollArea->setWidget(m_barsContainer);
    m_mainLayout->addWidget(m_scrollArea);
}

void StatisticsGoalsViewWidget::clearBars()
{
    QLayoutItem* item;
    while ((item = m_barsLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }
}

void StatisticsGoalsViewWidget::addBar(const QString& label, double hours, const QColor& color)
{
    StatisticsBarWidget* barWidget = new StatisticsBarWidget(label, hours, m_maxHours, color, this);
    m_barsLayout->addWidget(barWidget);
}

void StatisticsGoalsViewWidget::setSummaryText(const QString& text)
{
    m_summaryLabel->setText(text);
}