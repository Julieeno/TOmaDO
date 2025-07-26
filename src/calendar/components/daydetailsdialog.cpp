#include "calendar/components/daydetailsdialog.h"
#include "calendar/components/calendarutils.h"
#include "calendar/components/calendarstyles.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QListWidgetItem>
#include <QFont>

const QString DayDetailsDialog::DIALOG_STYLE = QString(
    "QDialog {"
    "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 %1, stop:1 %2);"
    "   border: 2px solid %3;"
    "   border-radius: 15px;"
    "}"
).arg(CalendarStyles::Colors::GRADIENT_START, CalendarStyles::Colors::GRADIENT_END, CalendarStyles::Colors::BORDER_LIGHT);

const QString DayDetailsDialog::DATE_LABEL_STYLE = QString(
    "QLabel {"
    "   color: %1;"
    "   background-color: rgba(255, 255, 255, 0.8);"
    "   border-radius: 8px;"
    "   padding: 15px;"
    "   border: 1px solid %2;"
    "}"
).arg(CalendarStyles::Colors::TEXT_SECONDARY, CalendarStyles::Colors::BORDER_LIGHT);

const QString DayDetailsDialog::LIST_STYLE = QString(
    "QListWidget {"
    "   background-color: rgba(255, 255, 255, 0.9);"
    "   border: 1px solid %1;"
    "   border-radius: 8px;"
    "   padding: 5px;"
    "}"
    "QListWidget::item {"
    "   border: none;"
    "   padding: 8px;"
    "   border-radius: 4px;"
    "   margin: 2px;"
    "}"
    "QListWidget::item:hover {"
    "   background-color: rgba(255, 179, 179, 0.3);"
    "}"
).arg(CalendarStyles::Colors::BORDER_LIGHT);

const QString DayDetailsDialog::CLOSE_BUTTON_STYLE = QString(
    "QPushButton {"
    "   background-color: %1;"
    "   color: white;"
    "   border: none;"
    "   border-radius: 8px;"
    "   padding: 12px 24px;"
    "   font-size: 14px;"
    "   font-weight: bold;"
    "}"
    "QPushButton:hover {"
    "   background-color: %2;"
    "}"
    "QPushButton:pressed {"
    "   background-color: %3;"
    "}"
).arg(CalendarStyles::Colors::BUTTON_PRIMARY, CalendarStyles::Colors::BUTTON_HOVER, CalendarStyles::Colors::BUTTON_PRESSED);

DayDetailsDialog::DayDetailsDialog(QWidget* parent)
    : QDialog(parent)
    , m_layout(nullptr)
    , m_dateLabel(nullptr)
    , m_detailsList(nullptr)
    , m_closeButton(nullptr)
{
    setupUi();
    setupStyling();
    setModal(true);
    resize(400, 500);
}

void DayDetailsDialog::setupUi()
{
    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(20, 20, 20, 20);
    m_layout->setSpacing(15);

    // Date header
    m_dateLabel = new QLabel(this);
    QFont dateFont = m_dateLabel->font();
    dateFont.setPointSize(16);
    dateFont.setBold(true);
    m_dateLabel->setFont(dateFont);
    m_dateLabel->setAlignment(Qt::AlignCenter);
    m_layout->addWidget(m_dateLabel);

    // Details list
    m_detailsList = new QListWidget(this);
    m_layout->addWidget(m_detailsList);

    // Close button
    m_closeButton = new QPushButton("Close", this);
    connect(m_closeButton, &QPushButton::clicked, this, &QDialog::accept);
    m_layout->addWidget(m_closeButton);
}

void DayDetailsDialog::setupStyling()
{
    setStyleSheet(DIALOG_STYLE);
    m_dateLabel->setStyleSheet(DATE_LABEL_STYLE);
    m_detailsList->setStyleSheet(LIST_STYLE);
    m_closeButton->setStyleSheet(CLOSE_BUTTON_STYLE);
}

void DayDetailsDialog::setData(const QDate& date, const QList<GoalTimeInfo>& goals, const QList<TodoInfo>& todos)
{
    m_date = date;
    m_goals = goals;
    m_todos = todos;

    QString formattedDate = CalendarUtils::formatDate(date);
    setWindowTitle(QString("Details for %1").arg(formattedDate));
    m_dateLabel->setText(formattedDate);

    populateList();
}

void DayDetailsDialog::populateList()
{
    m_detailsList->clear();
    addGoalsSection();
    addTodosSection();
}

void DayDetailsDialog::addGoalsSection()
{
    if (m_goals.isEmpty()) return;

    QListWidgetItem* goalHeader = new QListWidgetItem("🎯 Time Summary");
    QFont headerFont = goalHeader->font();
    headerFont.setPointSize(14);
    headerFont.setBold(true);
    goalHeader->setFont(headerFont);
    goalHeader->setFlags(goalHeader->flags() & ~Qt::ItemIsSelectable);
    m_detailsList->addItem(goalHeader);

    for (const auto& goal : m_goals) {
        QString goalText = QString("  %1: %2")
                          .arg(goal.goalTitle)
                          .arg(CalendarUtils::formatDuration(goal.minutes * 60));

        QListWidgetItem* goalItem = new QListWidgetItem(goalText);
        goalItem->setForeground(QBrush(goal.goalColor));
        QFont goalFont = goalItem->font();
        goalFont.setPointSize(12);
        goalItem->setFont(goalFont);
        goalItem->setFlags(goalItem->flags() & ~Qt::ItemIsSelectable);
        m_detailsList->addItem(goalItem);
    }
}

void DayDetailsDialog::addTodosSection()
{
    if (m_todos.isEmpty()) return;

    if (!m_goals.isEmpty()) {
        m_detailsList->addItem(new QListWidgetItem(""));
    }

    QListWidgetItem* todoHeader = new QListWidgetItem("📝 Todos");
    QFont headerFont = todoHeader->font();
    headerFont.setPointSize(14);
    headerFont.setBold(true);
    todoHeader->setFont(headerFont);
    todoHeader->setFlags(todoHeader->flags() & ~Qt::ItemIsSelectable);
    m_detailsList->addItem(todoHeader);

    for (const auto& todo : m_todos) {
        QString statusIcon = todo.isCompleted ? "✅" : "⭕";
        QString todoText = QString("  %1 %2")
                          .arg(statusIcon)
                          .arg(todo.todoTitle);

        QListWidgetItem* todoItem = new QListWidgetItem(todoText);
        if (todo.isCompleted) {
            QFont font = todoItem->font();
            font.setStrikeOut(true);
            todoItem->setFont(font);
            todoItem->setForeground(QColor::fromRgb(128, 128, 128));
        }
        todoItem->setFlags(todoItem->flags() & ~Qt::ItemIsSelectable);
        m_detailsList->addItem(todoItem);
    }
}