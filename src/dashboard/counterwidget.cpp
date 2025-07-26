
#include "dashboard/counterwidget.h"

// Define static constants
const QString CounterWidget::BLUE_COLOR = "#3498db";
const QString CounterWidget::GREEN_COLOR = "#27ae60";
const QString CounterWidget::ORANGE_COLOR = "#e67e22";
const QString CounterWidget::RED_COLOR = "#e74c3c";

const QString CounterWidget::BASE_STYLE = "QFrame { background-color: transparent; border-radius: 8px; padding: 8px; }";
const QString CounterWidget::TITLE_STYLE = "color: %1; font-weight: bold; font-size: 14px; text-align: center;";
const QString CounterWidget::VALUE_STYLE = "color: #2c3e50; font-size: 28px; text-align: center;";

CounterWidget::CounterWidget(const QString& title, const QString& value, const QString& color, QWidget* parent)
    : QFrame(parent), m_title(title), m_color(color), m_titleLabel(nullptr), m_valueLabel(nullptr)
{
    setFrameStyle(QFrame::Box);
    setupUi();
    updateValue(value);
}

void CounterWidget::setupUi() {
    // Set size policies
    setMinimumHeight(80);
    setMinimumWidth(120);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // Apply style
    setStyleSheet(getStyleForColor(m_color));

    // Create layout
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignCenter);
    layout->setContentsMargins(15, 15, 15, 15);
    layout->setSpacing(8);

    // Create and add title label
    m_titleLabel = new QLabel(m_title, this);
    m_titleLabel->setStyleSheet(QString(TITLE_STYLE).arg(m_color));
    m_titleLabel->setAlignment(Qt::AlignCenter);
    m_titleLabel->setWordWrap(true);
    layout->addWidget(m_titleLabel);

    // Create and add value label
    m_valueLabel = new QLabel(this);
    m_valueLabel->setStyleSheet(VALUE_STYLE);
    m_valueLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(m_valueLabel);
}

QString CounterWidget::getStyleForColor(const QString& color) const {
    QString backgroundStyle;

    if (color == BLUE_COLOR) {
        backgroundStyle = "QFrame { background-color: #e8f4f8; padding: 8px;" + BLUE_COLOR + "; }";
    } else if (color == GREEN_COLOR) {
        backgroundStyle = "QFrame { background-color: #e8f6f3; padding: 8px;" + GREEN_COLOR + "; }";
    } else if (color == ORANGE_COLOR) {
        backgroundStyle = "QFrame { background-color: #fdf2e9; padding: 8px;" + ORANGE_COLOR + "; }";
    } else if (color == RED_COLOR) {
        backgroundStyle = "QFrame { background-color: #ffeaa7; 8px; padding: 8px;" + RED_COLOR + "; }";
    } else {
        // Default fallback
        backgroundStyle = "QFrame { background-color: #f8f9fa; padding: 8px;" + color + "; }";
    }

    return backgroundStyle;
}

void CounterWidget::updateValue(const QString& value) {
    if (m_valueLabel) {
        m_valueLabel->setText(value);
    }
}