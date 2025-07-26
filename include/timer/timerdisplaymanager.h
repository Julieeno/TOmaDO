#ifndef TIMERDISPLAYMANAGER_H
#define TIMERDISPLAYMANAGER_H

#include <QObject>
#include <QLabel>
#include <QString>

class TimerDisplayManager : public QObject {
    Q_OBJECT

public:
    explicit TimerDisplayManager(QObject* parent = nullptr);
    ~TimerDisplayManager() override = default;

    void setUIComponents(QLabel* compactTimeLabel,
                        QLabel* compactTaskLabel,
                        QLabel* expandedTimeLabel,
                        QLabel* taskTitleLabel);

    void updateTimeDisplays(int remainingSeconds);

    void updateTaskDisplays(int goalId, const QString& title, const QString& description);

    QString formatTime(int seconds) const;

    void clearDisplays();

private:
    // UI Components
    QLabel* m_compactTimeLabel;
    QLabel* m_compactTaskLabel;
    QLabel* m_expandedTimeLabel;
    QLabel* m_taskTitleLabel;
};

#endif // TIMERDISPLAYMANAGER_H