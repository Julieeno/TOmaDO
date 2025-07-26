#include "statistics/statisticswidget.h"
#include "statistics/components/statisticsheaderwidget.h"
#include "statistics/components/statisticstimelyviewwidget.h"
#include "statistics/components/statisticsgoalsviewwidget.h"
#include "statistics/components/statisticsdatamanager.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QScrollArea>
#include <QFrame>
#include <QPainter>
#include <QPainterPath>
#include <QDate>
#include <QFont>
#include <QFontMetrics>
#include <QToolTip>
#include <QApplication>
#include <QLinearGradient>
#include <QMouseEvent>
#include <QPropertyAnimation>
#include <QGraphicsDropShadowEffect>
#include <QHelpEvent>
#include <cmath>
#include <QGroupBox>
#include <QTimer>
#include <QMenu>
#include <QActionGroup>
#include <QAction>
#include "database/databasemanager.h"

// StatisticsBarWidget implementation
StatisticsBarWidget::StatisticsBarWidget(const QString& label, double hours, double maxHours,
                                       const QColor& color, QWidget* parent)
    : QWidget(parent)
    , m_label(label)
    , m_hours(hours)
    , m_maxHours(maxHours)
    , m_barColor(color)
{
    setFixedHeight(BAR_HEIGHT + MARGIN * 2);
    setMinimumWidth(300);

    // Apply modern styling with rounded corners
    setStyleSheet(
        "StatisticsBarWidget {"
        "   background-color: #ffffff;"
        "   border-radius: 12px;"
        "   border: 1px solid #e0e0e0;"
        "}"
        "StatisticsBarWidget:hover {"
        "   background-color: #fefefe;"
        "   border: 1px solid #d0d0d0;"
        "}"
    );

    // Add subtle shadow effect
    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(8);
    shadow->setXOffset(0);
    shadow->setYOffset(2);
    shadow->setColor(QColor(0, 0, 0, 20));
    setGraphicsEffect(shadow);
}

void StatisticsBarWidget::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRect rect = this->rect();

    // Calculate areas
    QRect labelRect(0, 0, LABEL_WIDTH, rect.height());
    QRect barRect(LABEL_WIDTH + MARGIN, MARGIN,
                  rect.width() - LABEL_WIDTH - 2 * MARGIN,
                  rect.height() - 2 * MARGIN);

    // Draw label
    painter.setPen(QPen(QColor("#2c3e50"), 1));
    QFont labelFont = painter.font();
    labelFont.setPointSize(11);
    labelFont.setWeight(QFont::Medium);
    painter.setFont(labelFont);
    painter.drawText(labelRect, Qt::AlignLeft | Qt::AlignVCenter, m_label);

    // Draw bar background
    painter.setPen(QPen(QColor("#e0e0e0"), 1));
    painter.setBrush(QBrush(QColor("#f8f9fa")));
    painter.drawRoundedRect(barRect, 4, 4);

    // Draw actual bar if there's data
    if (m_hours > 0 && m_maxHours > 0) {
        double fillRatio = m_hours / m_maxHours;
        int fillWidth = static_cast<int>(fillRatio * barRect.width());

        QRect fillRect(barRect.left(), barRect.top(), fillWidth, barRect.height());

        painter.setPen(QPen(m_barColor.darker(110), 1));
        painter.setBrush(QBrush(m_barColor));
        painter.drawRoundedRect(fillRect, 4, 4);

        // Draw time text with h/m units
        painter.setPen(QPen(QColor("#ffffff")));
        QFont timeFont = painter.font();
        timeFont.setPointSize(10);
        timeFont.setWeight(QFont::Bold);
        painter.setFont(timeFont);

        // Format time text with units
        QString timeText;
        if (m_hours < 1.0) {
            int minutes = static_cast<int>(m_hours * 60);
            timeText = QString("%1m").arg(minutes);
        } else if (m_hours < 10.0) {
            timeText = QString("%1h").arg(QString::number(m_hours, 'f', 1));
        } else {
            timeText = QString("%1h").arg(static_cast<int>(m_hours));
        }

        // Only draw text if there's enough space
        QFontMetrics fm(timeFont);
        QRect textBounds = fm.boundingRect(timeText);
        if (fillWidth > textBounds.width() + 10) {
            // Draw text with shadow for better readability
            painter.setPen(QPen(QColor("#000000"), 1));
            painter.drawText(fillRect.adjusted(1, 1, 1, 1), Qt::AlignCenter, timeText);
            painter.setPen(QPen(QColor("#ffffff"), 1));
            painter.drawText(fillRect, Qt::AlignCenter, timeText);
        }
    }
}

// StatisticsVerticalBarWidget implementation
StatisticsVerticalBarWidget::StatisticsVerticalBarWidget(
    const QString& dateLabel,
    double maxHours,
    const QList<Segment>& segments,
    QWidget* parent)
    : QWidget(parent)
    , m_dateLabel(dateLabel)
    , m_segments(segments)
    , m_maxHours(maxHours > 0 ? maxHours : 1.0)
{
    setFixedWidth(BAR_WIDTH + MARGIN * 2);
    setToolTip(QString("Click to see details"));

    // Apply modern styling with rounded corners
    setStyleSheet(
        "StatisticsVerticalBarWidget {"
        "   background-color: #ffffff;"
        "   border-radius: 12px;"
        "   border: 1px solid #e0e0e0;"
        "}"
        "StatisticsVerticalBarWidget:hover {"
        "   background-color: #fefefe;"
        "   border: 1px solid #FF6B6B;"
        "   border-width: 2px;"
        "}"
    );

    // Add subtle shadow effect
    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(6);
    shadow->setXOffset(0);
    shadow->setYOffset(2);
    shadow->setColor(QColor(0, 0, 0, 15));
    setGraphicsEffect(shadow);
}

void StatisticsVerticalBarWidget::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRect rect = this->rect();

    // Reserve space at the top for total time display
    const int TOTAL_TIME_HEIGHT = 20;

    // Calculate areas with much larger bar area and smaller label area
    QRect totalTimeRect(0, 0, rect.width(), TOTAL_TIME_HEIGHT);
    QRect labelRect(0, rect.height() - 30, rect.width(), 30); // Reduced label height
    QRect barRect(MARGIN, TOTAL_TIME_HEIGHT + MARGIN, rect.width() - 2 * MARGIN,
                  rect.height() - 30 - TOTAL_TIME_HEIGHT - 3 * MARGIN); // Much larger bar area

    // Draw date label at bottom
    painter.setPen(QPen(QColor("#2c3e50"), 1));
    QFont labelFont = painter.font();
    labelFont.setPointSize(9); // Smaller font for more compact label
    labelFont.setWeight(QFont::Medium);
    painter.setFont(labelFont);
    painter.drawText(labelRect, Qt::AlignCenter, m_dateLabel);

    // Calculate total hours for this bar
    double totalHours = 0.0;
    for (const auto& segment : m_segments) {
        if (segment.hours > 0) {
            totalHours += segment.hours;
        }
    }

    // Draw total time at the top if there are segments
    if (totalHours > 0) {
        painter.setPen(QPen(QColor("#2c3e50"), 1));
        QFont totalFont = painter.font();
        totalFont.setPointSize(11);
        totalFont.setWeight(QFont::Bold);
        painter.setFont(totalFont);

        // Format total time inline
        QString totalText;
        if (totalHours < 1.0) {
            int minutes = static_cast<int>(totalHours * 60);
            totalText = QString("%1m").arg(minutes);
        } else if (totalHours < 10.0) {
            totalText = QString("%1h").arg(QString::number(totalHours, 'f', 1));
        } else {
            totalText = QString("%1h").arg(static_cast<int>(totalHours));
        }

        painter.drawText(totalTimeRect, Qt::AlignCenter, totalText);
    }

    // Draw bar background
    painter.setPen(QPen(QColor("#e0e0e0"), 1));
    painter.setBrush(QBrush(QColor("#f8f9fa")));
    painter.drawRoundedRect(barRect, 4, 4);

    // Only draw segments if we have data
    if (m_segments.isEmpty() || totalHours <= 0) {
        return;
    }

    double scaledTotalHours = sqrt(totalHours);
    double scaledMaxHours = sqrt(m_maxHours);

    double barFillRatio = scaledTotalHours / scaledMaxHours;
    double usedBarHeight = barFillRatio * barRect.height();

    double currentY = barRect.bottom();

    for (const auto& segment : m_segments) {
        if (segment.hours <= 0) continue;

        double proportion = segment.hours / totalHours;
        double segmentHeight = proportion * usedBarHeight;
        segmentHeight = qMax(segmentHeight, 3.0);

        QRect segmentRect(barRect.left(),
                         currentY - segmentHeight,
                         barRect.width(),
                         segmentHeight);

        painter.setPen(QPen(segment.color.darker(110), 1));
        painter.setBrush(QBrush(segment.color));
        painter.drawRoundedRect(segmentRect, 2, 2);

        currentY -= segmentHeight;
    }
}

bool StatisticsVerticalBarWidget::event(QEvent* event) {
    if (event->type() == QEvent::ToolTip) {
        if (!m_segments.isEmpty()) {
            QString tooltip = QString("<b>%1</b><br/>").arg(m_dateLabel);
            double totalHours = 0;

            for (const auto& segment : m_segments) {
                totalHours += segment.hours;
                tooltip += QString("• %1: %2h<br/>")
                          .arg(segment.goalTitle)
                          .arg(segment.hours, 0, 'f', 1);
            }

            tooltip += QString("<b>Total: %1h</b>").arg(totalHours, 0, 'f', 1);
            QToolTip::showText(static_cast<QHelpEvent*>(event)->globalPos(), tooltip);
        }
        return true;
    }
    return QWidget::event(event);
}

QSize StatisticsVerticalBarWidget::sizeHint() const {
    return QSize(BAR_WIDTH, 300);
}

int StatisticsVerticalBarWidget::getSegmentIndexAtPosition(const QPoint& pos) const {
    QRect rect = this->rect().adjusted(MARGIN, MARGIN, -MARGIN, -MARGIN);
    QRect barArea(rect.left() + 10, rect.top() + 10,
                  BAR_WIDTH - 20, rect.height() - LABEL_HEIGHT - 20);

    if (!barArea.contains(pos)) {
        return -1;
    }

    int relativeY = barArea.bottom() - pos.y();
    double ratio = static_cast<double>(relativeY) / barArea.height();
    double currentRatio = 0;

    for (int i = 0; i < m_segments.size(); ++i) {
        double segmentRatio = m_segments[i].hours / m_maxHours;
        if (ratio <= currentRatio + segmentRatio) {
            return i;
        }
        currentRatio += segmentRatio;
    }

    return -1;
}

// StatisticsWidget implementation
StatisticsWidget::StatisticsWidget(QWidget* parent) : QWidget(parent) {
    setupUi();
    connectSignals();
    refreshFromDatabase();
}

void StatisticsWidget::setupUi() {
    setStyleSheet(
        "StatisticsWidget {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "       stop:0 #f8f9fa, stop:1 #e9ecef);"
        "}"
        "QScrollArea {"
        "   border: none;"
        "   background-color: transparent;"
        "}"
        "QScrollBar:vertical {"
        "   background-color: #f1f3f4;"
        "   width: 8px;"
        "   border-radius: 4px;"
        "   margin: 0px;"
        "}"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {"
        "   height: 0px;"
        "   width: 0px;"
        "   subcontrol-origin: margin;"
        "   subcontrol-position: bottom;"
        "}"
        "QScrollBar::handle:vertical {"
        "   background-color: #FF6B6B;"
        "   border-radius: 4px;"
        "   min-height: 20px;"
        "}"
        "QScrollBar::handle:vertical:hover {"
        "   background-color: #FF5252;"
        "}"
    );

    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(20, 20, 20, 20);
    m_mainLayout->setSpacing(20);

    // Create data manager
    m_dataManager = new StatisticsDataManager(this);

    // Create header widget - now uses custom paintEvent for rounded corners
    m_headerWidget = new StatisticsHeaderWidget(this);
    m_mainLayout->addWidget(m_headerWidget);

    // Create timely view widget - now uses custom paintEvent for rounded corners
    m_timelyViewWidget = new StatisticsTimelyViewWidget(this);
    m_mainLayout->addWidget(m_timelyViewWidget);

    // Create goals view widget - now uses custom paintEvent for rounded corners
    m_goalsViewWidget = new StatisticsGoalsViewWidget(this);
    m_mainLayout->addWidget(m_goalsViewWidget);
}

void StatisticsWidget::connectSignals() {
    // Connect header widget signals
    connect(m_headerWidget, &StatisticsHeaderWidget::timeRangeChanged,
            this, &StatisticsWidget::onTimeRangeChanged);
    connect(m_headerWidget, &StatisticsHeaderWidget::refreshClicked,
            this, &StatisticsWidget::onRefreshClicked);
    connect(m_headerWidget, &StatisticsHeaderWidget::previousPeriod,
            this, &StatisticsWidget::onPreviousPeriod);
    connect(m_headerWidget, &StatisticsHeaderWidget::nextPeriod,
            this, &StatisticsWidget::onNextPeriod);
    connect(m_headerWidget, &StatisticsHeaderWidget::todayClicked,
            this, &StatisticsWidget::onTodayClicked);

    // Connect data manager signals
    connect(m_dataManager, &StatisticsDataManager::timeBasedDataLoaded,
            this, &StatisticsWidget::onTimeBasedDataLoaded);
    connect(m_dataManager, &StatisticsDataManager::goalDataLoaded,
            this, &StatisticsWidget::onGoalDataLoaded);
    connect(m_dataManager, &StatisticsDataManager::dataRefreshed,
            this, &StatisticsWidget::updateStatistics);
}

void StatisticsWidget::refreshFromDatabase() {
    m_dataManager->refreshFromDatabase();
}

void StatisticsWidget::onTimeRangeChanged() {
    updateStatistics();
}

void StatisticsWidget::onRefreshClicked() {
    refreshFromDatabase();
}

void StatisticsWidget::onPreviousPeriod() {
    QString rangeType = m_headerWidget->getCurrentTimeRange();
    QDate currentDate = m_headerWidget->getCurrentReferenceDate();

    if (rangeType == "daily") {
        currentDate = currentDate.addDays(-1);
    } else if (rangeType == "weekly") {
        currentDate = currentDate.addDays(-7);
    } else if (rangeType == "monthly") {
        currentDate = currentDate.addMonths(-1);
    }

    m_headerWidget->setCurrentReferenceDate(currentDate);
    m_headerWidget->updateNavigationButtons();
    updateStatistics();
}

void StatisticsWidget::onNextPeriod() {
    QString rangeType = m_headerWidget->getCurrentTimeRange();
    QDate currentDate = m_headerWidget->getCurrentReferenceDate();

    if (rangeType == "daily") {
        currentDate = currentDate.addDays(1);
    } else if (rangeType == "weekly") {
        currentDate = currentDate.addDays(7);
    } else if (rangeType == "monthly") {
        currentDate = currentDate.addMonths(1);
    }

    m_headerWidget->setCurrentReferenceDate(currentDate);
    m_headerWidget->updateNavigationButtons();
    updateStatistics();
}

void StatisticsWidget::onTodayClicked() {
    m_headerWidget->setCurrentReferenceDate(QDate::currentDate());
    m_headerWidget->updateNavigationButtons();
    updateStatistics();
}

void StatisticsWidget::onTimeBasedDataLoaded(const QList<QPair<QString, QList<StatisticsVerticalBarWidget::Segment>>>& data,
                                           const QString& summaryText) {
    // Clear existing bars
    m_timelyViewWidget->clearVerticalBars();

    // Set max hours for scaling
    m_timelyViewWidget->setMaxVerticalHours(m_dataManager->getMaxVerticalHours());

    // Add new bars
    for (const auto& barData : data) {
        m_timelyViewWidget->addVerticalBar(barData.first, m_dataManager->getMaxVerticalHours(), barData.second);
    }

    // Update summary
    m_timelyViewWidget->setSummaryText(summaryText);
}

void StatisticsWidget::onGoalDataLoaded(const QList<QPair<QString, QPair<double, QColor>>>& data,
                                      const QString& summaryText) {
    // Clear existing bars
    m_goalsViewWidget->clearBars();

    // Set max hours for scaling
    m_goalsViewWidget->setMaxHours(m_dataManager->getMaxHours());

    // Add new bars
    for (const auto& barData : data) {
        m_goalsViewWidget->addBar(barData.first, barData.second.first, barData.second.second);
    }

    // Update summary
    m_goalsViewWidget->setSummaryText(summaryText);
}

void StatisticsWidget::updateStatistics() {
    QString rangeType = m_headerWidget->getCurrentTimeRange();
    QDate referenceDate = m_headerWidget->getCurrentReferenceDate();

    // Update header
    m_headerWidget->updatePeriodLabel();
    m_headerWidget->updateNavigationButtons();

    // Load data through data manager
    m_dataManager->loadTimeBasedStatistics(rangeType, referenceDate);
    m_dataManager->loadGoalStatistics(rangeType, referenceDate);
}