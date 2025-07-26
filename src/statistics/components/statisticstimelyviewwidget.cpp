#include "statistics/components/statisticstimelyviewwidget.h"
#include <QFrame>
#include <QPainter>
#include <QPainterPath>
#include <QLinearGradient>

StatisticsTimelyViewWidget::StatisticsTimelyViewWidget(QWidget* parent)
    : QWidget(parent)
    , m_mainLayout(nullptr)
    , m_titleLabel(nullptr)
    , m_summaryLabel(nullptr)
    , m_scrollArea(nullptr)
    , m_verticalBarsContainer(nullptr)
    , m_verticalBarsLayout(nullptr)
    , m_maxVerticalHours(0.0)
{
    setupUi();
}

void StatisticsTimelyViewWidget::paintEvent(QPaintEvent* event)
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

void StatisticsTimelyViewWidget::setupUi()
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
    m_titleLabel = new QLabel("Time Distribution", this);
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

    // Scroll area for vertical bars
    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setMinimumHeight(250);
    m_scrollArea->setStyleSheet(
        "QScrollArea {"
        "   border: none;"
        "   background-color: transparent;"
        "}"
    );

    m_verticalBarsContainer = new QWidget();
    m_verticalBarsContainer->setStyleSheet("QWidget { background-color: transparent; }");
    m_verticalBarsLayout = new QHBoxLayout(m_verticalBarsContainer);
    m_verticalBarsLayout->setSpacing(15);
    m_verticalBarsLayout->setContentsMargins(15, 15, 15, 15);
    m_verticalBarsLayout->setAlignment(Qt::AlignCenter);

    m_scrollArea->setWidget(m_verticalBarsContainer);
    m_mainLayout->addWidget(m_scrollArea);
}

void StatisticsTimelyViewWidget::clearVerticalBars()
{
    QLayoutItem* item;
    while ((item = m_verticalBarsLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }
}

void StatisticsTimelyViewWidget::addVerticalBar(const QString& label, double maxHours, 
                                              const QList<StatisticsVerticalBarWidget::Segment>& segments)
{
    StatisticsVerticalBarWidget* barWidget = new StatisticsVerticalBarWidget(label, maxHours, segments, this);
    m_verticalBarsLayout->addWidget(barWidget);
}

void StatisticsTimelyViewWidget::setSummaryText(const QString& text)
{
    m_summaryLabel->setText(text);
}