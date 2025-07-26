#ifndef GOALDIALOG_H
#define GOALDIALOG_H

#include <QDialog>
#include "database/databasemanager.h"

class GoalDialogFormManager;
class GoalDialogUI;

class GoalDialog : public QDialog {
    Q_OBJECT

public:
    explicit GoalDialog(const DatabaseManager::GoalItem& goal, QWidget* parent = nullptr);
    ~GoalDialog() override;

    DatabaseManager::GoalItem getGoal() const;

private slots:
    void onAccept();
    void onColorChanged(const QString& colorCode);
    void onValidationFailed(const QString& message, QWidget* focusWidget);

private:
    void setupDialog();
    void setupUI();
    void setupFormManager();
    void connectSignals();
    void configureDialog();

    QString getDialogTitle() const;
    void showValidationError(const QString& message, QWidget* focusWidget);

    // Data
    DatabaseManager::GoalItem m_originalGoal;

    // Managers
    GoalDialogFormManager* m_formManager;

    // UI state
    bool m_isEditMode;
};

#endif // GOALDIALOG_H