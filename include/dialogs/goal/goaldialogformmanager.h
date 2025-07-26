#ifndef GOALDIALOGFORMMANAGER_H
#define GOALDIALOGFORMMANAGER_H

#include <QObject>
#include <QWidget>
#include "database/databasemanager.h"

class QLineEdit;
class QTextEdit;
class QComboBox;
class QSpinBox;
class QCheckBox;
class ColorPickerWidget;

class GoalDialogFormManager : public QObject {
    Q_OBJECT

public:
    explicit GoalDialogFormManager(QWidget* parent = nullptr);

    void setFormWidgets(QLineEdit* titleEdit, QTextEdit* descriptionEdit,
                       QComboBox* priorityCombo, QSpinBox* timeSpinBox,
                       QCheckBox* completionCheckbox, ColorPickerWidget* colorPicker);

    void populateForm(const DatabaseManager::GoalItem& goal);
    DatabaseManager::GoalItem extractGoalData(const DatabaseManager::GoalItem& originalGoal) const;

    bool validateForm();
    void focusFirstField();

    QString getSelectedColor() const { return m_selectedColor; }

    signals:
        void validationFailed(const QString& message, QWidget* focusWidget);
    void colorChanged(const QString& colorCode);

private slots:
    void onColorSelected(const QString& colorCode);

private:
    void setupPriorityComboBox();
    void setupTimeSpinBox();
    void setupFormConnections();

    int convertSecondsToMinutes(int seconds) const;
    int convertMinutesToSeconds(int minutes) const;

    // Form widgets
    QLineEdit* m_titleEdit;
    QTextEdit* m_descriptionEdit;
    QComboBox* m_priorityCombo;
    QSpinBox* m_timeSpinBox;
    QCheckBox* m_completionCheckbox;
    ColorPickerWidget* m_colorPicker;

    // State
    QString m_selectedColor;
    QWidget* m_parentWidget;
};

#endif // GOALDIALOGFORMMANAGER_H