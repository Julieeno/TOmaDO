#include "timer/soundmanager.h"
#include "settings/settingsdialog.h"
#include <QApplication>
#include <QTimer>
#include <QProcess>
#include <QFileInfo>
#include <QSettings>
#include <QDebug>
#include <QCoreApplication>
#include <QDir>

SoundManager& SoundManager::instance()
{
    static SoundManager instance;
    return instance;
}

SoundManager::SoundManager(QObject* parent)
    : QObject(parent)
{
}

void SoundManager::playNotificationSound()
{
    if (!SettingsDialog::getDefaultNotificationEnabled()) {
        return;
    }

    playSystemSound(SettingsDialog::getDefaultNotificationSound());
}

void SoundManager::playSystemSound(const QString& soundType)
{
    if (soundType == "single" || soundType == "system") {
        playSystemBeep();
    } else if (soundType == "double") {
        playSystemBeep();
        QTimer::singleShot(200, this, &SoundManager::playSystemBeep);
    } else if (soundType == "triple") {
        playSystemBeep();
        QTimer::singleShot(200, this, &SoundManager::playSystemBeep);
        QTimer::singleShot(400, this, &SoundManager::playSystemBeep);
    } else if (soundType == "alert") {
        if (!playSystemAlert()) {
            playSystemBeep();
        }
    } else if (!soundType.isEmpty()) {
        playCustomSound(soundType);
    }
}

void SoundManager::playSystemBeep()
{
    QApplication::beep();
}

bool SoundManager::playSystemAlert()
{
    QString soundType = SettingsDialog::getDefaultNotificationSound();

    if (QFileInfo::exists(soundType)) {
        return tryPlaySound(soundType);
    }

    QStringList commands;
    commands << "canberra-gtk-play -i dialog-information"
             << "notify-send 'Timer' 'Timer finished!'";

    for (const QString& command : commands) {
        QStringList args = command.split(' ');
        QString program = args.takeFirst();

        if (QProcess::startDetached(program, args)) {
            return true;
        }
    }

    return false;
}

void SoundManager::playTimerEndSound()
{
    if (!SettingsDialog::getDefaultNotificationEnabled()) {
        qDebug() << "Timer end sound disabled in settings";
        return;
    }

    QString soundPath = getTimerEndSoundPath();
    qDebug() << "Timer end sound setting:" << soundPath;

    if (!soundPath.isEmpty()) {
        tryPlaySound(soundPath);
    } else {
        qDebug() << "No timer end sound configured";
    }
}

void SoundManager::playBreakEndSound()
{
    if (!SettingsDialog::getDefaultNotificationEnabled()) {
        qDebug() << "Break end sound disabled in settings";
        return;
    }

    QString soundPath = getBreakEndSoundPath();
    qDebug() << "Break end sound setting:" << soundPath;

    if (!soundPath.isEmpty()) {
        tryPlaySound(soundPath);
    } else {
        qDebug() << "No break end sound configured";
    }
}

QString SoundManager::getTimerEndSoundPath() const
{
    QSettings settings("TOmaDO", "Settings");
    QString soundFile = settings.value("timer_end_sound", "alarm.mp3").toString();

    QStringList searchPaths;
    searchPaths << QCoreApplication::applicationDirPath() + "/sounds/"
                << QDir::homePath() + "/.local/share/tomado/sounds/"
                << "/usr/local/share/tomado/sounds/"
                << "/usr/share/tomado/sounds/"
                << "sounds/";

    for (const QString& path : searchPaths) {
        QString fullPath = path + soundFile;
        qDebug() << "Checking for timer sound at:" << fullPath;
        if (QFileInfo::exists(fullPath)) {
            return fullPath;
        }
    }

    qDebug() << "Timer end sound file not found:" << soundFile;
    return "";
}

QString SoundManager::getBreakEndSoundPath() const
{
    QSettings settings("TOmaDO", "Settings");
    QString soundFile = settings.value("break_end_sound", "bell.mp3").toString();

    QStringList searchPaths;
    searchPaths << QCoreApplication::applicationDirPath() + "/sounds/"
                << QDir::homePath() + "/.local/share/tomado/sounds/"
                << "/usr/local/share/tomado/sounds/"
                << "/usr/share/tomado/sounds/"
                << "sounds/";

    for (const QString& path : searchPaths) {
        QString fullPath = path + soundFile;
        qDebug() << "Checking for break sound at:" << fullPath;
        if (QFileInfo::exists(fullPath)) {
            return fullPath;
        }
    }

    qDebug() << "Break end sound file not found:" << soundFile;
    return "";
}

QStringList SoundManager::getSoundPlayerCommands(const QString& soundPath) const
{
    QString fileExtension = QFileInfo(soundPath).suffix().toLower();
    QStringList commands;

    if (fileExtension == "mp3") {
        commands.clear();
        commands << QString("mpg123 \"%1\"").arg(soundPath)
                << QString("ffplay -nodisp -autoexit \"%1\"").arg(soundPath)
                << QString("cvlc --play-and-exit \"%1\"").arg(soundPath);
    } else if (fileExtension == "wav") {
        commands.clear();
        commands << QString("aplay \"%1\"").arg(soundPath)
                << QString("paplay \"%1\"").arg(soundPath)
                << QString("ffplay -nodisp -autoexit \"%1\"").arg(soundPath);
    } else {
        commands.clear();
        commands << QString("ffplay -nodisp -autoexit \"%1\"").arg(soundPath)
                << QString("cvlc --play-and-exit \"%1\"").arg(soundPath);
    }

    return commands;
}

bool SoundManager::tryPlaySound(const QString& soundPath)
{
    if (soundPath.isEmpty()) {
        qDebug() << "Empty sound path provided";
        return false;
    }

    if (!QFileInfo::exists(soundPath)) {
        qDebug() << "Sound file does not exist:" << soundPath;
        return false;
    }

    qDebug() << "Attempting to play sound:" << soundPath;

    QStringList commands = getSoundPlayerCommands(soundPath);

    for (const QString& command : commands) {
        QStringList args = command.split(' ');
        QString program = args.takeFirst();

        for (QString& arg : args) {
            if (arg.startsWith('"') && arg.endsWith('"')) {
                arg = arg.mid(1, arg.length() - 2);
            }
        }

        if (QProcess::startDetached(program, args)) {
            qDebug() << "Successfully started player:" << program;
            return true;
        }
    }

    qDebug() << "No audio player could play the file:" << soundPath;
    return false;
}

void SoundManager::playCustomSound(const QString& soundPath)
{
    tryPlaySound(soundPath);
}