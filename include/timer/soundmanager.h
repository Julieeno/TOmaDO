#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include <QObject>
#include <QString>
#include <QStringList>

class SoundManager : public QObject {
    Q_OBJECT

public:
    static SoundManager& instance();

    void playNotificationSound();

public slots:
    void playTimerEndSound();
    void playBreakEndSound();
    void playCustomSound(const QString& soundPath);

private:
    explicit SoundManager(QObject* parent = nullptr);
    ~SoundManager() = default;

    // Sound playing methods
    void playSystemBeep();
    void playSystemSound(const QString& soundType);
    bool playSystemAlert();
    bool tryPlaySound(const QString& soundPath);

    // Helper methods
    QString getTimerEndSoundPath() const;
    QString getBreakEndSoundPath() const;

    QStringList getSoundPlayerCommands(const QString& soundPath) const;
};

#endif // SOUNDMANAGER_H