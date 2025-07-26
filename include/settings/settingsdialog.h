#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QSettings>
#include <QGroupBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>

class SettingsDialog : public QDialog {
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget* parent = nullptr);
    ~SettingsDialog() override = default;

    // Settings access interface
    static int getDefaultTimerDuration();
    static int getDefaultBreakDuration();
    static int getDefaultLongBreakDuration();
    static int getDefaultTimersBeforeLongBreak();
    static QString getDefaultNotificationSound();
    static bool getDefaultNotificationEnabled();
    static void saveTimerDuration(int minutes);
    static void saveBreakDuration(int minutes);
    static void saveLongBreakDuration(int minutes);
    static void saveTimersBeforeLongBreak(int timers);
    static void saveNotificationSound(const QString& soundPath);
    static void saveNotificationEnabled(bool enabled);

signals:
    void settingsChanged();
    void databaseUpdated();

private slots:
    void onSaveClicked();
    void onCancelClicked();
    void onEraseGoalStatsClicked();
    void onEraseTodosClicked();
    void onRestoreDefaultsClicked();
    void onTestTimerEndSoundClicked();
    void onTestBreakEndSoundClicked();
    void onNotificationEnabledChanged(bool enabled);

private:
    // UI setup methods
    void setupUi();
    void setupLayout();
    void setupStyling();
    void createTimerSettingsGroup();
    void createNotificationSettingsGroup();
    void createDataManagementGroup();
    void createButtonBox();
    void connectSignals();

    // Settings methods
    void loadSettings();
    void saveSettings();
    void resetToDefaults();
    void populateAvailableSounds();
    void updateSoundControlsState();

    // Helper methods
    void playTestSound(const QString& soundPath);
    bool showConfirmationDialog(const QString& title, const QString& message);
    void showSuccessMessage(const QString& message);
    void showErrorMessage(const QString& message);

    // UI Components
    QVBoxLayout* m_mainLayout;

    // Timer Settings Group
    QGroupBox* m_timerGroup;
    QSpinBox* m_timerDurationSpinBox;
    QSpinBox* m_breakDurationSpinBox;
    QSpinBox* m_longBreakDurationSpinBox;
    QSpinBox* m_timersBeforeLongBreakSpinBox;

    // Notification Settings Group
    QGroupBox* m_notificationGroup;
    QCheckBox* m_notificationEnabledCheckBox;
    QComboBox* m_timerEndSoundComboBox;
    QComboBox* m_breakEndSoundComboBox;
    QPushButton* m_testTimerEndSoundButton;
    QPushButton* m_testBreakEndSoundButton;

    // Data Management Group
    QGroupBox* m_dataManagementGroup;
    QPushButton* m_eraseGoalStatsButton;
    QPushButton* m_eraseTodosButton;

    // Action Buttons
    QPushButton* m_saveButton;
    QPushButton* m_cancelButton;
    QPushButton* m_restoreDefaultsButton;

    // Settings storage
    QSettings m_settings;

    // Style constants
    static const QString DIALOG_STYLE;
    static const QString GROUP_STYLE;
    static const QString BUTTON_STYLE;
    static const QString DANGER_BUTTON_STYLE;
    static const QString INPUT_STYLE;
    static const QString CHECKBOX_STYLE;
};

#endif // SETTINGSDIALOG_H