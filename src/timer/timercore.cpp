#include "timer/timercore.h"
#include <QDebug>
#include <QSettings>

TimerCore::TimerCore(QObject* parent)
    : QObject(parent)
    , m_timer(std::make_unique<QTimer>(this))
    , m_state(State::Idle)
{
    // Setup timer
    m_timer->setInterval(TIMER_INTERVAL_MS);
    connect(m_timer.get(), &QTimer::timeout, this, &TimerCore::onTimerTick);

    // Load settings and initialize session data
    QSettings settings("TOmaDO", "Settings");
    int workTime = settings.value("timer_duration", DEFAULT_WORK_TIME).toInt();
    int breakTime = settings.value("break_duration", DEFAULT_BREAK_TIME).toInt();
    int longBreakTime = settings.value("long_break_duration", DEFAULT_LONG_BREAK_TIME).toInt();

    m_sessionData.workTimeMinutes = workTime;
    m_sessionData.breakTimeMinutes = breakTime;
    m_sessionData.longBreakTimeMinutes = longBreakTime;
    m_sessionData.remainingSeconds = workTime * 60;
}

void TimerCore::startTimer() {
    if (m_state == State::Idle) {
        if (m_sessionData.isBreakTime) {
            setupBreakSession();
            setState(State::Break);
        } else {
            setupWorkSession();
            setState(State::Running);
        }
        m_timer->start();
    } else if (m_state == State::Paused) {
        if (m_sessionData.pauseStartTime.isValid()) {
            QDateTime now = QDateTime::currentDateTime();
            int pausedSeconds = m_sessionData.pauseStartTime.secsTo(now);
            m_sessionData.totalPausedSeconds += pausedSeconds;
            m_sessionData.pauseStartTime = QDateTime();
        }
        if (m_sessionData.isBreakTime) {
            setState(State::Break);
        } else {
            setState(State::Running);
        }
        m_timer->start();
    }
}

void TimerCore::pauseTimer() {
    if (m_state == State::Running || m_state == State::Break) {
        m_timer->stop();
        m_sessionData.pauseStartTime = QDateTime::currentDateTime();
        setState(State::Paused);
    }
}

void TimerCore::setLongBreakTime(int minutes) {
    m_sessionData.longBreakTimeMinutes = minutes;
}

void TimerCore::startBreakTimer() {
    m_sessionData.completedTimersCount++;
    m_sessionData.isLongBreak = shouldTakeLongBreak();

    m_sessionData.isBreakTime = true;
    m_sessionData.remainingSeconds = calculateBreakTime() * 60;

    setState(State::Break);
    m_timer->start();
}

int TimerCore::calculateBreakTime() const {
    if (m_sessionData.isLongBreak) {
        return m_sessionData.longBreakTimeMinutes;
    }
    return m_sessionData.breakTimeMinutes;
}

void TimerCore::stopTimer() {
    if (m_state == State::Running || m_state == State::Paused || m_state == State::Break) {
        m_timer->stop();

        if (m_sessionData.startTime.isValid() && !m_sessionData.isBreakTime) {
            QDateTime endTime = QDateTime::currentDateTime();

            SessionData dataToSave = m_sessionData;
            dataToSave.pauseStartTime = endTime;
            emit sessionDataReady(dataToSave);
        }
        resetTimer();
    }
}

void TimerCore::completeWorkSession() {
    SessionData dataToSave = m_sessionData;
    dataToSave.pauseStartTime = QDateTime::currentDateTime();
    emit sessionDataReady(dataToSave);

    // Clear start time to prepare for break
    m_sessionData.startTime = QDateTime();

    emit workSessionCompleted();
    startBreakTimer();
}

void TimerCore::resetTimer() {
    m_timer->stop();

    m_sessionData.goalId = -1;
    m_sessionData.title.clear();
    m_sessionData.description.clear();
    m_sessionData.startTime = QDateTime();
    m_sessionData.pauseStartTime = QDateTime();
    m_sessionData.totalPausedSeconds = 0;
    m_sessionData.remainingSeconds = m_sessionData.workTimeMinutes * 60;
    m_sessionData.isBreakTime = false;

    setState(State::Idle);
    emit timerReset();
}

void TimerCore::startWithGoal(int goalId, const QString& title, const QString& description) {
    m_sessionData.goalId = goalId;
    m_sessionData.title = title;
    m_sessionData.description = description;
}

void TimerCore::startQuickTimer() {
    m_sessionData.goalId = -1;
    m_sessionData.title = "Quick Timer";
    m_sessionData.description = "Quick timer session";
}

void TimerCore::setWorkTime(int minutes) {
    m_sessionData.workTimeMinutes = minutes;
    if (m_state == State::Idle && !m_sessionData.isBreakTime) {
        m_sessionData.remainingSeconds = minutes * 60;
    }
}

void TimerCore::setBreakTime(int minutes) {
    m_sessionData.breakTimeMinutes = minutes;
    if (m_state == State::Idle && m_sessionData.isBreakTime) {
        m_sessionData.remainingSeconds = minutes * 60;
    }
}

void TimerCore::finishBreakTimer() {
    completeBreakSession();
    resetTimer();
}

bool TimerCore::shouldTakeLongBreak() const {
    return (m_sessionData.completedTimersCount > 0) && 
           (m_sessionData.completedTimersCount % SESSIONS_FOR_LONG_BREAK == 0);
}

void TimerCore::onTimerTick() {
    if (m_sessionData.remainingSeconds > 0) {
        m_sessionData.remainingSeconds--;
        emit timeUpdated(m_sessionData.remainingSeconds);
    } else {
        m_timer->stop();
        
        if (m_sessionData.isBreakTime) {
            completeBreakSession();
        } else {
            completeWorkSession();
        }
    }
}

void TimerCore::setState(State newState) {
    if (m_state != newState) {
        m_state = newState;
        emit stateChanged(newState);
    }
}

void TimerCore::setupWorkSession() {
    m_sessionData.isBreakTime = false;
    m_sessionData.remainingSeconds = m_sessionData.workTimeMinutes * 60;
    m_sessionData.startTime = QDateTime::currentDateTime();
    m_sessionData.pauseStartTime = QDateTime();
    m_sessionData.totalPausedSeconds = 0;
}

void TimerCore::setupBreakSession() {
    m_sessionData.isBreakTime = true;
    m_sessionData.remainingSeconds = calculateBreakTime() * 60;

    if (m_state == State::Idle) {
        m_sessionData.startTime = QDateTime::currentDateTime();
        m_sessionData.pauseStartTime = QDateTime();
        m_sessionData.totalPausedSeconds = 0;
    }
}

void TimerCore::completeBreakSession() {
    m_sessionData.isBreakTime = false;
    m_sessionData.isLongBreak = false;

    m_sessionData.remainingSeconds = m_sessionData.workTimeMinutes * 60;
    m_sessionData.startTime = QDateTime();
    m_sessionData.pauseStartTime = QDateTime();
    m_sessionData.totalPausedSeconds = 0;

    emit breakSessionCompleted();
    setState(State::Idle);
}