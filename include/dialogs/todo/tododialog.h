#ifndef TODODIALOG_H
#define TODODIALOG_H

#include <QDialog>
#include "database/databasemanager.h"

class TodoDialogFormManager;
class TodoDialogUI;

class TodoDialog : public QDialog {
    Q_OBJECT

public:
    explicit TodoDialog(const DatabaseManager::TodoItem& todo = DatabaseManager::TodoItem(), QWidget* parent = nullptr);
    explicit TodoDialog(int preselectedGoalId, QWidget* parent = nullptr);

    DatabaseManager::TodoItem getTodo() const;
    void setPreselectedGoal(int goalId, const QString& goalTitle = QString());

private slots:
    void onAccept();
    void onCancel();
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
    DatabaseManager::TodoItem m_originalTodo;
    int m_preselectedGoalId;
    bool m_isEditMode;

    // Components
    TodoDialogFormManager* m_formManager;
};

#endif // TODODIALOG_H