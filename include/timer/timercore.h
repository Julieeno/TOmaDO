#ifndef TIMERCORE_H
#define TIMERCORE_H

#include <QObject>
#include <QTimer>
#include <QDateTime>
#include <QString>
#include <memory>


class TimerCore : public QObject {
    Q_OBJECT

public:
    enum class State {
        Idle,      ///< Timer is not running
        Running,   ///< Timer is running
        Paused,    ///< Timer is paused
        Break      ///< In break time
    };

    struct SessionData {
        int goalId = -1;
        QString title;
        QString description;
        QDateTime startTime;
        QDateTime pauseStartTime;
        int totalPausedSeconds = 0;
        int workTimeMinutes = 25;
        int breakTimeMinutes = 5;
        int longBreakTimeMinutes = 15;
        int remainingSeconds = 0;
        bool isBreakTime = false;
        int completedTimersCount = 0;
        bool isLongBreak = false;
    };

    explicit TimerCore(QObject* parent = nullptr);
    ~TimerCore() override = default;

    // State queries
    State currentState() const { return m_state; }
    bool isRunning() const { return m_state == State::Running || m_state == State::Break; }
    bool isPaused() const { return m_state == State::Paused; }
    bool isBreakTime() const { return m_sessionData.isBreakTime; }
    bool isIdle() const { return m_state == State::Idle; }

    // Session data access
    const SessionData& sessionData() const { return m_sessionData; }
    int remainingSeconds() const { return m_sessionData.remainingSeconds; }
    int currentGoalId() const { return m_sessionData.goalId; }
    QString currentTitle() const { return m_sessionData.title; }
    QString currentDescription() const { return m_sessionData.description; }
    int completedTimersCount() const { return m_sessionData.completedTimersCount; }

    // Timer control
    void startTimer();
    void pauseTimer();
    void stopTimer();
    void resetTimer();

    // Session setup
    void startWithGoal(int goalId, const QString& title, const QString& description);
    void startQuickTimer();
    void setWorkTime(int minutes);
    void setBreakTime(int minutes);
    void setLongBreakTime(int minutes);

    // Break management
    void startBreakTimer();
    void finishBreakTimer();
    bool shouldTakeLongBreak() const;

signals:
    void stateChanged(TimerCore::State newState);

    void timeUpdated(int remainingSeconds);

    void workSessionCompleted();

    void breakSessionCompleted();

    void timerReset();

    void sessionDataReady(const SessionData& data);

private slots:
    void onTimerTick();

private:
    // Constants
    static constexpr int TIMER_INTERVAL_MS = 1000;
    static constexpr int DEFAULT_WORK_TIME = 25;
    static constexpr int DEFAULT_BREAK_TIME = 5;
    static constexpr int DEFAULT_LONG_BREAK_TIME = 15;
    static constexpr int SESSIONS_FOR_LONG_BREAK = 4;

    void setState(State newState);
    void setupWorkSession();
    void setupBreakSession();
    void completeWorkSession();
    void completeBreakSession();
    int calculateBreakTime() const;

    // Core timer
    std::unique_ptr<QTimer> m_timer;
    State m_state;
    SessionData m_sessionData;
};

#endif // TIMERCORE_H