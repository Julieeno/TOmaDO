#include "timer/timerdisplaymanager.h"
#include <QLabel>
#include <QSettings>

TimerDisplayManager::TimerDisplayManager(QObject* parent)
    : QObject(parent)
    , m_compactTimeLabel(nullptr)
    , m_compactTaskLabel(nullptr)
    , m_expandedTimeLabel(nullptr)
    , m_taskTitleLabel(nullptr)
{
}

void TimerDisplayManager::setUIComponents(QLabel* compactTimeLabel,
                                         QLabel* compactTaskLabel,
                                         QLabel* expandedTimeLabel,
                                         QLabel* taskTitleLabel) {
    m_compactTimeLabel = compactTimeLabel;
    m_compactTaskLabel = compactTaskLabel;
    m_expandedTimeLabel = expandedTimeLabel;
    m_taskTitleLabel = taskTitleLabel;
}

void TimerDisplayManager::updateTimeDisplays(int remainingSeconds) {
    QString timeText = formatTime(remainingSeconds);

    if (m_compactTimeLabel) {
        m_compactTimeLabel->setText(timeText);
    }

    if (m_expandedTimeLabel) {
        m_expandedTimeLabel->setText(timeText);
    }
}

void TimerDisplayManager::updateTaskDisplays(int goalId, const QString& title, const QString& description) {
    QString displayText;

    if (goalId == -1) {
        displayText = "Quick Timer";
    } else if (!title.isEmpty()) {
        displayText = title;
    } else {
        displayText = "Timer Session";
    }

    if (m_compactTaskLabel) {
        m_compactTaskLabel->setText(displayText);
    }

    if (m_taskTitleLabel) {
        m_taskTitleLabel->setText(displayText);
    }
}

QString TimerDisplayManager::formatTime(int seconds) const {
    int minutes = seconds / 60;
    int remainingSeconds = seconds % 60;
    return QString("%1:%2").arg(minutes, 2, 10, QChar('0')).arg(remainingSeconds, 2, 10, QChar('0'));
}

void TimerDisplayManager::clearDisplays() {
    // Load the actual timer duration from settings instead of hardcoded value
    QSettings settings("TOmaDO", "Settings");
    int defaultMinutes = settings.value("timer_duration", 25).toInt();
    QString defaultTimeText = QString("%1:00").arg(defaultMinutes, 2, 10, QChar('0'));

    if (m_compactTimeLabel) {
        m_compactTimeLabel->setText(defaultTimeText);
    }

    if (m_expandedTimeLabel) {
        m_expandedTimeLabel->setText(defaultTimeText);
    }
    
    if (m_compactTaskLabel) {
        m_compactTaskLabel->setText("");
    }
    
    if (m_taskTitleLabel) {
        m_taskTitleLabel->setText("");
    }
}