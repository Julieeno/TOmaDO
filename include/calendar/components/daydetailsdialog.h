#ifndef DAYDETAILSDIALOG_H
#define DAYDETAILSDIALOG_H

#include <QDialog>
#include <QDate>
#include <QList>
#include "customcalendarwidget.h"

class QVBoxLayout;
class QLabel;
class QListWidget;
class QPushButton;

class DayDetailsDialog : public QDialog {
    Q_OBJECT

public:
    explicit DayDetailsDialog(QWidget* parent = nullptr);
    
    void setData(const QDate& date, const QList<GoalTimeInfo>& goals, const QList<TodoInfo>& todos);

private:
    void setupUi();
    void setupStyling();
    void populateList();
    void addGoalsSection();
    void addTodosSection();
    
    // UI Components
    QVBoxLayout* m_layout;
    QLabel* m_dateLabel;
    QListWidget* m_detailsList;
    QPushButton* m_closeButton;
    
    // Data
    QDate m_date;
    QList<GoalTimeInfo> m_goals;
    QList<TodoInfo> m_todos;
    
    // Constants
    static const QString DIALOG_STYLE;
    static const QString DATE_LABEL_STYLE;
    static const QString LIST_STYLE;
    static const QString CLOSE_BUTTON_STYLE;
};

#endif // DAYDETAILSDIALOG_H