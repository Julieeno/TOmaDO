#ifndef COMPACTTIMERWIDGET_H
#define COMPACTTIMERWIDGET_H

#include <QWidget>
#include <QTimer>
#include <QDateTime>
#include <QStackedWidget>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QMainWindow>
#include <memory>

#include "timer/soundmanager.h"
#include "timer/lavalamppaintwidget.h"
#include "timer/timerbuttonmanager.h"
#include "timer/timercore.h"
#include "timer/timergoalsmanager.h"
#include "timer/timerdisplaymanager.h"

class QLabel;
class QPushButton;
class QLineEdit;
class QSpinBox;
class QVBoxLayout;
class QHBoxLayout;
class QComboBox;

class CompactTimerWidget : public QWidget {
    Q_OBJECT
    Q_PROPERTY(qreal transitionProgress READ transitionProgress WRITE setTransitionProgress)

public:
    explicit CompactTimerWidget(QWidget* parent = nullptr);

    ~CompactTimerWidget() override = default;

    void setMainWindowComponents(QStackedWidget* stackedWidget, QWidget* centralWidget,
                                QWidget* mainWidget, QHBoxLayout* mainLayout);

    qreal transitionProgress() const { return m_transitionProgress; }

    void setTransitionProgress(qreal progress);

signals:
    void databaseUpdated();

    void clicked();

    void expandedStateChanged(bool isExpanded);

public slots:
    void startTimerWithGoal(int goalId, const QString& title, const QString& description);

    void startQuickTimer();

    void refreshGoalsList();

    void refreshSettings();

    void setSelectedGoal(int goalId);

protected:
    void enterEvent(QEvent* event) override;

    void leaveEvent(QEvent* event) override;

    void resizeEvent(QResizeEvent* event) override;

    void mousePressEvent(QMouseEvent* event) override;

private slots:
    void startTimer();

    void stopTimer();

    void pauseTimer();

    void toggleTimer();

    void expandTimer();

    void collapseTimer();

    void onTransitionFinished();

    // Timer core event handlers
    void onTimerStateChanged(TimerCore::State newState);
    void onTimerTimeUpdated(int remainingSeconds);
    void onWorkSessionCompleted();
    void onBreakSessionCompleted();
    void onTimerReset();
    void onSessionDataReady(const TimerCore::SessionData& data);
    void endBreakTimer();

    // Goals manager event handlers
    void onGoalSelectionChanged(int goalId, const QString& title, const QString& description);

private:
    // Constants
    static constexpr int TRANSITION_DURATION_MS = 300;

    void setupCompactUi();

    void setupExpandedUi();

    void setupExpandedSettings(QVBoxLayout* expandedLayout);

    void setupLavaLampBackground();

    void updateLavaLampColors();

    void saveToDatabase(const TimerCore::SessionData& data);

    void updateButtonStates();

    void initializeCompactMode();

    void switchToCompactMode();

    void switchToExpandedMode();

    void startCompactTransition();

    void startExpandedTransition();

    void updateTodayTimeSpent();

    void setWidgetOpacity(QWidget* widget, qreal opacity);

    void connectTimerCore();

    void connectGoalsManager();

    void startBreakTimer();

    void finishBreakTimer();

    bool shouldTakeLongBreak() const;

    void updateGoalHeader();
    void updateDisplay();

    // Core logic
    std::unique_ptr<TimerCore> m_timerCore;

    // Helper managers
    std::unique_ptr<TimerButtonManager> m_buttonManager;
    std::unique_ptr<TimerGoalsManager> m_goalsManager;
    std::unique_ptr<TimerDisplayManager> m_displayManager;

    // UI state
    bool isExpanded;

    // UI Elements
    QWidget* compactWidget;
    QWidget* expandedWidget;
    QVBoxLayout* mainLayout;

    // References to main window components
    QStackedWidget* m_stackedWidget;
    QWidget* m_centralWidget;
    QWidget* m_mainWidget;
    QHBoxLayout* m_mainLayout;
    QVBoxLayout* m_originalMainLayout;
    QWidget* m_sidebarWidget;

    // Compact UI
    QLabel* compactTimeLabel;
    QLabel* compactTaskLabel;

    // Expanded UI
    QLabel* expandedTimeLabel;
    QLineEdit* descriptionEdit;
    QSpinBox* workTimeSpinBox;
    QSpinBox* breakTimeSpinBox;
    QLabel* taskTitleLabel;
    QComboBox* goalSelectionCombo;
    QLabel* todayTimeSpentLabel;
    QLabel* goalHeaderLabel;

    // Transition properties
    QPropertyAnimation* m_transitionAnimation;
    qreal m_transitionProgress;
    bool m_isTransitioning;

    // Geometry data for transitions
    QRect m_startGeometry;
    QRect m_endGeometry;
    QWidget* m_timerArea;

    // Lava lamp background
    LavaLampPaintWidget* m_lavaLampWidget;
};

#endif // COMPACTTIMERWIDGET_H