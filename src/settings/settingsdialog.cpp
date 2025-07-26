#include "settings/settingsdialog.h"
#include "database/databasemanager.h"
#include "timer/soundmanager.h"
#include "ui/dialogstyles.h"
#include <QMessageBox>
#include <QDir>
#include <QFileInfo>
#include <QDebug>

// Style constants
const QString SettingsDialog::DIALOG_STYLE =
    "QDialog {"
    "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
    "       stop:0 #FFFBFB, stop:0.5 #FFF5F5, stop:1 #FFEFEF);"
    "   border-radius: 12px;"
    "}";

const QString SettingsDialog::GROUP_STYLE =
    "QGroupBox {"
    "   font-weight: 600;"
    "   font-size: 16px;"
    "   color: #2c3e50;"
    "   border: 2px solid #FFE0E0;"
    "   border-radius: 12px;"
    "   margin-top: 12px;"
    "   padding-top: 15px;"
    "   background: rgba(255, 255, 255, 80);"
    "}"
    "QGroupBox::title {"
    "   subcontrol-origin: margin;"
    "   left: 15px;"
    "   padding: 5px 12px;"
    "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
    "       stop:0 #FFE5E5, stop:1 #FFF0F0);"
    "   border-radius: 6px;"
    "   border: 1px solid #FFD6D6;"
    "}";

const QString SettingsDialog::BUTTON_STYLE =
    "QPushButton {"
    "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
    "       stop:0 #FF6B7A, stop:1 #FF5A6B);"
    "   color: white;"
    "   border: none;"
    "   border-radius: 8px;"
    "   padding: 10px 20px;"
    "   font-weight: 600;"
    "   font-size: 14px;"
    "   min-width: 80px;"
    "}"
    "QPushButton:hover {"
    "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
    "       stop:0 #FF5A6B, stop:1 #FF4D5E);"
    "}"
    "QPushButton#saveButton {"
    "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
    "       stop:0 #27ae60, stop:1 #229954);"
    "}"
    "QPushButton#saveButton:hover {"
    "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
    "       stop:0 #229954, stop:1 #1e8449);"
    "}"
    "QPushButton#cancelButton {"
    "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
    "       stop:0 #bdc3c7, stop:1 #95a5a6);"
    "}"
    "QPushButton#cancelButton:hover {"
    "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
    "       stop:0 #95a5a6, stop:1 #7f8c8d);"
    "}";

const QString SettingsDialog::DANGER_BUTTON_STYLE =
    "QPushButton {"
    "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
    "       stop:0 #e74c3c, stop:1 #c0392b);"
    "   color: white;"
    "   border: none;"
    "   border-radius: 8px;"
    "   padding: 10px 20px;"
    "   font-weight: 600;"
    "   font-size: 14px;"
    "}"
    "QPushButton:hover {"
    "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
    "       stop:0 #c0392b, stop:1 #a93226);"
    "}";

const QString SettingsDialog::INPUT_STYLE =
    "QSpinBox, QComboBox {"
    "   background: rgba(255, 255, 255, 180);"
    "   border: 2px solid #FFE0E0;"
    "   border-radius: 8px;"
    "   padding: 8px 12px;"
    "   font-size: 14px;"
    "   color: #2c3e50;"
    "}"
    "QSpinBox:focus, QComboBox:focus {"
    "   border: 2px solid #FF6B7A;"
    "   background: rgba(255, 255, 255, 220);"
    "}"
    "QLabel {"
    "   color: #2c3e50;"
    "   font-weight: 600;"
    "   font-size: 14px;"
    "   background: transparent;"
    "}";

const QString SettingsDialog::CHECKBOX_STYLE =
    "QCheckBox {"
    "   color: #2c3e50;"
    "   font-weight: 500;"
    "   font-size: 14px;"
    "   spacing: 8px;"
    "}"
    "QCheckBox::indicator {"
    "   width: 18px;"
    "   height: 18px;"
    "   border: 2px solid #FFE0E0;"
    "   border-radius: 4px;"
    "   background: rgba(255, 255, 255, 180);"
    "}"
    "QCheckBox::indicator:checked {"
    "   background: #FF6B7A;"
    "   border: 2px solid #FF6B7A;"
    "}"
    "QCheckBox::indicator:hover {"
    "   border: 2px solid #FF6B7A;"
    "}";

SettingsDialog::SettingsDialog(QWidget* parent)
    : QDialog(parent)
    , m_mainLayout(nullptr)
    , m_timerGroup(nullptr)
    , m_timerDurationSpinBox(nullptr)
    , m_breakDurationSpinBox(nullptr)
    , m_longBreakDurationSpinBox(nullptr)
    , m_timersBeforeLongBreakSpinBox(nullptr)
    , m_notificationGroup(nullptr)
    , m_notificationEnabledCheckBox(nullptr)
    , m_timerEndSoundComboBox(nullptr)
    , m_breakEndSoundComboBox(nullptr)
    , m_testTimerEndSoundButton(nullptr)
    , m_testBreakEndSoundButton(nullptr)
    , m_dataManagementGroup(nullptr)
    , m_eraseGoalStatsButton(nullptr)
    , m_eraseTodosButton(nullptr)
    , m_saveButton(nullptr)
    , m_cancelButton(nullptr)
    , m_restoreDefaultsButton(nullptr)
    , m_settings("TOmaDO", "Settings")
{
    setupUi();
    loadSettings();
}

void SettingsDialog::setupUi()
{
    setWindowTitle(tr("Settings"));
    setModal(true);
    setMinimumSize(500, 600);

    setupLayout();
    setupStyling();

    createTimerSettingsGroup();
    createNotificationSettingsGroup();
    createDataManagementGroup();
    createButtonBox();

    connectSignals();

    m_mainLayout->addStretch();
}

void SettingsDialog::setupLayout()
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(20, 20, 20, 20);
    m_mainLayout->setSpacing(20);
}

void SettingsDialog::setupStyling()
{
    setStyleSheet(DIALOG_STYLE + GROUP_STYLE + BUTTON_STYLE + INPUT_STYLE + CHECKBOX_STYLE);
}

void SettingsDialog::createTimerSettingsGroup()
{
    m_timerGroup = new QGroupBox(tr("Timer Settings"), this);
    auto timerLayout = new QGridLayout(m_timerGroup);

    // Timer Duration
    auto timerDurationLabel = new QLabel(tr("Timer Duration (minutes):"), this);
    m_timerDurationSpinBox = new QSpinBox(this);
    m_timerDurationSpinBox->setRange(1, 120);
    m_timerDurationSpinBox->setValue(45);
    timerLayout->addWidget(timerDurationLabel, 0, 0);
    timerLayout->addWidget(m_timerDurationSpinBox, 0, 1);

    // Break Duration
    auto breakDurationLabel = new QLabel(tr("Break Duration (minutes):"), this);
    m_breakDurationSpinBox = new QSpinBox(this);
    m_breakDurationSpinBox->setRange(1, 60);
    m_breakDurationSpinBox->setValue(10);
    timerLayout->addWidget(breakDurationLabel, 1, 0);
    timerLayout->addWidget(m_breakDurationSpinBox, 1, 1);

    // Long Break Duration
    auto longBreakDurationLabel = new QLabel(tr("Long Break Duration (minutes):"), this);
    m_longBreakDurationSpinBox = new QSpinBox(this);
    m_longBreakDurationSpinBox->setRange(5, 120);
    m_longBreakDurationSpinBox->setValue(20);
    timerLayout->addWidget(longBreakDurationLabel, 2, 0);
    timerLayout->addWidget(m_longBreakDurationSpinBox, 2, 1);

    // Timers Before Long Break
    auto timersBeforeLongBreakLabel = new QLabel(tr("Timers Before Long Break:"), this);
    m_timersBeforeLongBreakSpinBox = new QSpinBox(this);
    m_timersBeforeLongBreakSpinBox->setRange(1, 10);
    m_timersBeforeLongBreakSpinBox->setValue(4);
    timerLayout->addWidget(timersBeforeLongBreakLabel, 3, 0);
    timerLayout->addWidget(m_timersBeforeLongBreakSpinBox, 3, 1);

    // Restore defaults button
    m_restoreDefaultsButton = new QPushButton(tr("Restore Defaults"), this);
    timerLayout->addWidget(m_restoreDefaultsButton, 4, 0, 1, 2);

    m_mainLayout->addWidget(m_timerGroup);
}

void SettingsDialog::createNotificationSettingsGroup()
{
    m_notificationGroup = new QGroupBox(tr("Sound Settings"), this);
    auto notificationLayout = new QGridLayout(m_notificationGroup);

    // Enable notifications checkbox
    m_notificationEnabledCheckBox = new QCheckBox(tr("Enable Sound Notifications"), this);
    m_notificationEnabledCheckBox->setChecked(true);
    notificationLayout->addWidget(m_notificationEnabledCheckBox, 0, 0, 1, 3);

    // Timer End Sound section
    auto timerEndSoundLabel = new QLabel(tr("Timer End Sound:"), this);
    m_timerEndSoundComboBox = new QComboBox(this);
    m_testTimerEndSoundButton = new QPushButton(tr("Test"), this);
    m_testTimerEndSoundButton->setFixedWidth(60);

    notificationLayout->addWidget(timerEndSoundLabel, 1, 0);
    notificationLayout->addWidget(m_timerEndSoundComboBox, 1, 1);
    notificationLayout->addWidget(m_testTimerEndSoundButton, 1, 2);

    // Break End Sound section
    auto breakEndSoundLabel = new QLabel(tr("Break End Sound:"), this);
    m_breakEndSoundComboBox = new QComboBox(this);
    m_testBreakEndSoundButton = new QPushButton(tr("Test"), this);
    m_testBreakEndSoundButton->setFixedWidth(60);

    notificationLayout->addWidget(breakEndSoundLabel, 2, 0);
    notificationLayout->addWidget(m_breakEndSoundComboBox, 2, 1);
    notificationLayout->addWidget(m_testBreakEndSoundButton, 2, 2);

    // Populate sound options
    populateAvailableSounds();

    m_mainLayout->addWidget(m_notificationGroup);
}

void SettingsDialog::createDataManagementGroup()
{
    m_dataManagementGroup = new QGroupBox(tr("Data Management"), this);
    auto dataLayout = new QVBoxLayout(m_dataManagementGroup);

    m_eraseGoalStatsButton = new QPushButton(tr("Erase All Goal Statistics"), this);
    m_eraseGoalStatsButton->setStyleSheet(DANGER_BUTTON_STYLE);
    dataLayout->addWidget(m_eraseGoalStatsButton);

    m_eraseTodosButton = new QPushButton(tr("Erase All Todos"), this);
    m_eraseTodosButton->setStyleSheet(DANGER_BUTTON_STYLE);
    dataLayout->addWidget(m_eraseTodosButton);

    m_mainLayout->addWidget(m_dataManagementGroup);
}

void SettingsDialog::createButtonBox()
{
    auto buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(10);

    m_cancelButton = new QPushButton(tr("Cancel"), this);
    m_cancelButton->setObjectName("cancelButton");

    m_saveButton = new QPushButton(tr("Save"), this);
    m_saveButton->setObjectName("saveButton");
    DialogStyles::configureStandaloneButton(m_saveButton);

    m_saveButton->setMinimumSize(80, 35);
    m_cancelButton->setMinimumSize(80, 35);

    buttonLayout->addStretch();
    buttonLayout->addWidget(m_cancelButton);
    buttonLayout->addWidget(m_saveButton);

    m_mainLayout->addLayout(buttonLayout);
}

void SettingsDialog::connectSignals()
{
    // Button connections
    connect(m_saveButton, &QPushButton::clicked, this, &SettingsDialog::onSaveClicked);
    connect(m_cancelButton, &QPushButton::clicked, this, &SettingsDialog::onCancelClicked);
    connect(m_restoreDefaultsButton, &QPushButton::clicked, this, &SettingsDialog::onRestoreDefaultsClicked);
    connect(m_eraseGoalStatsButton, &QPushButton::clicked, this, &SettingsDialog::onEraseGoalStatsClicked);
    connect(m_eraseTodosButton, &QPushButton::clicked, this, &SettingsDialog::onEraseTodosClicked);

    // Sound test connections
    connect(m_testTimerEndSoundButton, &QPushButton::clicked, this, &SettingsDialog::onTestTimerEndSoundClicked);
    connect(m_testBreakEndSoundButton, &QPushButton::clicked, this, &SettingsDialog::onTestBreakEndSoundClicked);

    // Notification state connection
    connect(m_notificationEnabledCheckBox, &QCheckBox::toggled, this, &SettingsDialog::onNotificationEnabledChanged);
}

void SettingsDialog::populateAvailableSounds()
{
    m_timerEndSoundComboBox->clear();
    m_breakEndSoundComboBox->clear();

    QDir soundsDir("sounds");
    QStringList soundFiles = soundsDir.entryList(QStringList() << "*.wav" << "*.mp3" << "*.ogg", QDir::Files);

    for (const QString& soundFile : soundFiles) {
        m_timerEndSoundComboBox->addItem(soundFile, soundFile);
        m_breakEndSoundComboBox->addItem(soundFile, soundFile);
    }

    if (soundFiles.isEmpty()) {
        m_timerEndSoundComboBox->addItem(tr("No sounds found"), "");
        m_breakEndSoundComboBox->addItem(tr("No sounds found"), "");
    }
}

void SettingsDialog::loadSettings()
{
    m_timerDurationSpinBox->setValue(m_settings.value("timer_duration", 45).toInt());
    m_breakDurationSpinBox->setValue(m_settings.value("break_duration", 10).toInt());
    m_longBreakDurationSpinBox->setValue(m_settings.value("long_break_duration", 20).toInt());
    m_timersBeforeLongBreakSpinBox->setValue(m_settings.value("timers_before_long_break", 4).toInt());

    bool notificationEnabled = m_settings.value("notification_enabled", true).toBool();
    m_notificationEnabledCheckBox->setChecked(notificationEnabled);

    QString timerEndSound = m_settings.value("timer_end_sound", "").toString();
    int timerIndex = m_timerEndSoundComboBox->findData(timerEndSound);
    if (timerIndex >= 0) {
        m_timerEndSoundComboBox->setCurrentIndex(timerIndex);
    }

    QString breakEndSound = m_settings.value("break_end_sound", "").toString();
    int breakIndex = m_breakEndSoundComboBox->findData(breakEndSound);
    if (breakIndex >= 0) {
        m_breakEndSoundComboBox->setCurrentIndex(breakIndex);
    }

    updateSoundControlsState();
}

void SettingsDialog::saveSettings()
{
    m_settings.setValue("timer_duration", m_timerDurationSpinBox->value());
    m_settings.setValue("break_duration", m_breakDurationSpinBox->value());
    m_settings.setValue("long_break_duration", m_longBreakDurationSpinBox->value());
    m_settings.setValue("timers_before_long_break", m_timersBeforeLongBreakSpinBox->value());
    m_settings.setValue("notification_enabled", m_notificationEnabledCheckBox->isChecked());
    m_settings.setValue("timer_end_sound", m_timerEndSoundComboBox->currentData().toString());
    m_settings.setValue("break_end_sound", m_breakEndSoundComboBox->currentData().toString());
    m_settings.sync();
}

void SettingsDialog::resetToDefaults()
{
    m_timerDurationSpinBox->setValue(45);
    m_breakDurationSpinBox->setValue(10);
    m_longBreakDurationSpinBox->setValue(20);
    m_timersBeforeLongBreakSpinBox->setValue(4);
    m_notificationEnabledCheckBox->setChecked(true);
    updateSoundControlsState();
}

void SettingsDialog::updateSoundControlsState()
{
    bool enabled = m_notificationEnabledCheckBox->isChecked();
    m_timerEndSoundComboBox->setEnabled(enabled);
    m_breakEndSoundComboBox->setEnabled(enabled);
    m_testTimerEndSoundButton->setEnabled(enabled);
    m_testBreakEndSoundButton->setEnabled(enabled);
}

void SettingsDialog::playTestSound(const QString& soundPath)
{
    if (!soundPath.isEmpty()) {
        QString fullPath = "sounds/" + soundPath;
        SoundManager::instance().playCustomSound(fullPath);
    }
}

bool SettingsDialog::showConfirmationDialog(const QString& title, const QString& message)
{
    QMessageBox confirmDialog(this);
    confirmDialog.setWindowTitle(title);
    confirmDialog.setText(message);
    confirmDialog.setInformativeText(tr("This action cannot be undone."));
    confirmDialog.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    confirmDialog.setDefaultButton(QMessageBox::No);
    confirmDialog.setIcon(QMessageBox::Warning);

    return confirmDialog.exec() == QMessageBox::Yes;
}

void SettingsDialog::showSuccessMessage(const QString& message)
{
    QMessageBox::information(this, tr("Success"), message);
}

void SettingsDialog::showErrorMessage(const QString& message)
{
    QMessageBox::critical(this, tr("Error"), message);
}

// Slot implementations
void SettingsDialog::onSaveClicked()
{
    saveSettings();
    emit settingsChanged();
    accept();
}

void SettingsDialog::onCancelClicked()
{
    reject();
}

void SettingsDialog::onRestoreDefaultsClicked()
{
    resetToDefaults();
}

void SettingsDialog::onEraseGoalStatsClicked()
{
    if (showConfirmationDialog(tr("Confirm Deletion"), tr("Are you sure you want to erase all goal statistics?"))) {
        if (DatabaseManager::instance().clearTimerRecords()) {
            showSuccessMessage(tr("All goal statistics have been erased."));
            emit databaseUpdated();
        } else {
            showErrorMessage(tr("Failed to erase goal statistics."));
        }
    }
}

void SettingsDialog::onEraseTodosClicked()
{
    if (showConfirmationDialog(tr("Confirm Deletion"), tr("Are you sure you want to erase all todos?"))) {
        if (DatabaseManager::instance().clearTodos()) {
            showSuccessMessage(tr("All todos have been erased."));
            emit databaseUpdated();
        } else {
            showErrorMessage(tr("Failed to erase todos."));
        }
    }
}

void SettingsDialog::onTestTimerEndSoundClicked()
{
    QString soundValue = m_timerEndSoundComboBox->currentData().toString();
    playTestSound(soundValue);
}

void SettingsDialog::onTestBreakEndSoundClicked()
{
    QString soundValue = m_breakEndSoundComboBox->currentData().toString();
    playTestSound(soundValue);
}

void SettingsDialog::onNotificationEnabledChanged(bool enabled)
{
    updateSoundControlsState();
}

// Static methods implementation
int SettingsDialog::getDefaultTimerDuration()
{
    return QSettings("TOmaDO", "Settings").value("timer_duration", 25).toInt();
}

int SettingsDialog::getDefaultBreakDuration()
{
    return QSettings("TOmaDO", "Settings").value("break_duration", 5).toInt();
}

int SettingsDialog::getDefaultLongBreakDuration()
{
    return QSettings("TOmaDO", "Settings").value("long_break_duration", 15).toInt();
}

int SettingsDialog::getDefaultTimersBeforeLongBreak()
{
    return QSettings("TOmaDO", "Settings").value("timers_before_long_break", 4).toInt();
}

QString SettingsDialog::getDefaultNotificationSound()
{
    QSettings settings("TOmaDO", "Settings");
    return settings.value("timer_end_sound", "").toString();
}

bool SettingsDialog::getDefaultNotificationEnabled()
{
    return QSettings("TOmaDO", "Settings").value("notification_enabled", true).toBool();
}

void SettingsDialog::saveTimerDuration(int minutes)
{
    QSettings("TOmaDO", "Settings").setValue("timer_duration", minutes);
}

void SettingsDialog::saveBreakDuration(int minutes)
{
    QSettings("TOmaDO", "Settings").setValue("break_duration", minutes);
}

void SettingsDialog::saveLongBreakDuration(int minutes)
{
    QSettings("TOmaDO", "Settings").setValue("long_break_duration", minutes);
}

void SettingsDialog::saveTimersBeforeLongBreak(int timers)
{
    QSettings("TOmaDO", "Settings").setValue("timers_before_long_break", timers);
}

void SettingsDialog::saveNotificationSound(const QString& soundPath)
{
    QSettings("TOmaDO", "Settings").setValue("notification_sound", soundPath);
}

void SettingsDialog::saveNotificationEnabled(bool enabled)
{
    QSettings("TOmaDO", "Settings").setValue("notification_enabled", enabled);
}