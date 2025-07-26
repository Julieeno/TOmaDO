#include "dashboard/basedashboardwidget.h"
#include "dashboard/counterwidget.h"
#include "../../include/dialogs/todo/tododialog.h"

#include <QShowEvent>
#include <QLabel>
#include <QListWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTimer>
#include <QListWidgetItem>
#include <QContextMenuEvent>
#include <QMenu>
#include <QAction>
#include <QDebug>
#include <QFrame>
#include <QPushButton>

namespace DashboardConstants {
    constexpr int LIST_UPDATE_BATCH_SIZE = 100;
    constexpr int PROGRESS_UPDATE_THRESHOLD = 5;
    constexpr int MAX_TODO_DISPLAY_LENGTH = 80;

    const QDate EARLIEST_DATE(2000, 1, 1);
    const QDate LATEST_DATE(2100, 12, 31);
}

// Modern tomato-based color constants
const QString BaseDashboardWidget::TITLE_STYLE =
    "QLabel {"
    "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
    "       stop:0 #FFF5F5, stop:1 #FFE8E8);"
    "   font-size: 30px;"
    "   font-weight: 700;"
    "   color: #A64545;"
    "   border-radius: 15px;"
    "   border: none;"
    "   min-height: 80px;"
    "   padding: 0px 12px;"
    "}";

const QString BaseDashboardWidget::DATE_STYLE =
    "QLabel {"
    "   font-size: 16px;"
    "   color: #7f8c8d;"
    "   font-weight: 500;"
    "}";

const QString BaseDashboardWidget::COUNTER_FRAME_STYLE =
    "QFrame {"
    "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 %1, stop:1 %2);"
    "   border-radius: 16px;"
    "   padding: 20px;"
    "   min-height: 80px;"
    "   min-width: 180px;"
    "}";

const QString BaseDashboardWidget::COUNTER_LABEL_STYLE =
    "QLabel {"
    "   color: %1;"
    "   font-weight: 600;"
    "   font-size: 13px;"
    "   text-transform: uppercase;"
    "   letter-spacing: 1px;"
    "   background: transparent;"
    "   padding: 0px;"
    "   margin: 0px;"
    "}";

const QString BaseDashboardWidget::COUNTER_VALUE_STYLE =
    "QLabel {"
    "   color: #2c3e50;"
    "   font-weight: 900;"
    "   font-size: 36px;"
    "   background: transparent;"
    "   padding: 0px;"
    "   margin: 0px;"
    "}";

const QString BaseDashboardWidget::TODO_HEADER_STYLE =
    "QLabel {"
    "   font-size: 20px;"
    "   font-weight: 700;"
    "   color: #2c3e50;"
    "   margin: 15px 0;"
    "   padding: 10px 15px;"
    "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
    "       stop:0 #FFE5E5, stop:1 #FFF0F0);"
    "   border-radius: 12px;"
    "   border-left: 4px solid #FF6B7A;"
    "}";

const QString BaseDashboardWidget::COMPLETED_HEADER_STYLE =
    "QLabel {"
    "   font-size: 18px;"
    "   font-weight: 600;"
    "   color: #2c3e50;"
    "   margin: 20px 0 15px 0;"
    "   padding: 8px 15px;"
    "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
    "       stop:0 #E8F5E8, stop:1 #F0F8F0);"
    "   border-radius: 10px;"
    "   border-left: 4px solid #27ae60;"
    "}";

// Localized strings
const QString BaseDashboardWidget::TODAY_PENDING_TODOS_TITLE = QObject::tr("Today's Pending To-dos");
const QString BaseDashboardWidget::TODAY_COMPLETED_TODOS_TITLE = QObject::tr("Today's Completed To-dos");
const QString BaseDashboardWidget::WEEK_PENDING_TODOS_TITLE = QObject::tr("This Week's Pending To-dos");
const QString BaseDashboardWidget::WEEK_COMPLETED_TODOS_TITLE = QObject::tr("This Week's Completed To-dos");
const QString BaseDashboardWidget::TOTAL_PENDING_TODOS_TITLE = QObject::tr("All Pending To-dos");
const QString BaseDashboardWidget::TOTAL_COMPLETED_TODOS_TITLE = QObject::tr("All Completed To-dos");

BaseDashboardWidget::BaseDashboardWidget(QWidget* parent)
    : QWidget(parent)
    , m_isBeingDestroyed(false)
    , m_mainLayout(nullptr)
    , m_titleLabel(nullptr)
    , m_dateLabel(nullptr)
    , m_statsWidget(nullptr)
    , m_statsLayout(nullptr)
    , m_pendingTodosHeaderLayout(nullptr)
    , m_pendingTodosListLabel(nullptr)
    , m_pendingTodosScrollArea(nullptr)
    , m_pendingTodosList(nullptr)
    , m_completedTodosListLabel(nullptr)
    , m_completedTodosScrollArea(nullptr)
    , m_completedTodosList(nullptr)
    , m_refreshTimer(nullptr)
    , m_pendingTodosCount(0)
    , m_completedTodosCount(0)
    , m_timeSpentSeconds(0)
    , m_timersCompletedCount(0)
{
    setupUi();
    connectSignals();

    // Create auto-refresh timer
    m_refreshTimer = new QTimer(this);
    m_refreshTimer->setInterval(REFRESH_INTERVAL_MS);
    connect(m_refreshTimer, &QTimer::timeout, this, &BaseDashboardWidget::refreshFromDatabase);
    m_refreshTimer->start();
}

BaseDashboardWidget::~BaseDashboardWidget()
{
    m_isBeingDestroyed = true;
    if (m_refreshTimer) {
        m_refreshTimer->stop();
    }
}

void BaseDashboardWidget::setupUi()
{
    // Main widget styling with modern tomato theme
    setStyleSheet(
        "BaseDashboardWidget {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "       stop:0 #FFFBFB, stop:0.5 #FFF5F5, stop:1 #FFEFEF);"
        "   border-radius: 20px;"
        "   border: 1px solid #FFD6D6;"
        "}"
        "QScrollArea {"
        "   background: transparent;"
        "   border: none;"
        "   border-radius: 12px;"
        "}"
        "QListWidget {"
        "   background: rgba(255, 255, 255, 200);"
        "   border: 2px solid #FFE0E0;"
        "   border-radius: 12px;"
        "   outline: none;"
        "}"
        "QListWidget::item {"
        "   background: rgba(255, 255, 255, 150);"
        "   border: 1px solid #FFE8E8;"
        "   border-radius: 8px;"
        "   font-size: 14px;"
        "   color: #2c3e50;"
        "}"
        "QListWidget::item:hover {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "       stop:0 #FFE5E5, stop:1 #FFF0F0);"
        "   border: 1px solid #FFB3B3;"
        "   transform: translateY(-2px);"
        "}"
        "QListWidget::item:selected {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "       stop:0 #FFB3B3, stop:1 #FFCCCC);"
        "   border: 2px solid #FF8A8A;"
        "   color: #2c3e50;"
        "   font-weight: 600;"
        "}"
        "QPushButton {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "       stop:0 #FF6B7A, stop:1 #FF5A6B);"
        "   color: white;"
        "   border: none;"
        "   border-radius: 10px;"
        "   padding: 10px 20px;"
        "   font-weight: 600;"
        "   font-size: 14px;"
        "}"
        "QPushButton:hover {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "       stop:0 #FF5A6B, stop:1 #FF4D5E);"
        "}"
        "QPushButton:pressed {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "       stop:0 #FF4D5E, stop:1 #FF3E51);"
        "}"
    );

    // Create main layout
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(20, 20, 20, 0);
    m_mainLayout->setSpacing(20);

    // Create title label
    m_titleLabel = new QLabel(this);
    m_titleLabel->setStyleSheet(TITLE_STYLE);
    m_titleLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_mainLayout->addWidget(m_titleLabel);

    // Create date label
    m_dateLabel = new QLabel(this);
    m_dateLabel->setStyleSheet(DATE_STYLE);
    m_dateLabel->setAlignment(Qt::AlignCenter);
    m_mainLayout->addWidget(m_dateLabel);

    // Setup counters section
    setupCounters();

    // Create todo sections
    createTodoHeader();
    createTodoLists();
}

void BaseDashboardWidget::setupCounters()
{
    // Create stats container with modern styling
    m_statsWidget = new QWidget(this);
    m_statsWidget->setStyleSheet(
        "QWidget {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "       stop:0 #FFFFFF, stop:1 #FFF8F8);"
        "   border-radius: 16px;"
        "   padding: 15px;"
        "}"
    );

    m_statsLayout = new QHBoxLayout(m_statsWidget);
    m_statsLayout->setContentsMargins(10, 10, 10, 10);
    m_statsLayout->setSpacing(15);

    m_mainLayout->addWidget(m_statsWidget);
}

void BaseDashboardWidget::createTodoHeader()
{
    // Create header container with add button
    QWidget* headerContainer = new QWidget(this);
    m_pendingTodosHeaderLayout = new QHBoxLayout(headerContainer);
    m_pendingTodosHeaderLayout->setContentsMargins(0, 0, 0, 0);
    m_pendingTodosHeaderLayout->setSpacing(15);

    // Create pending todos label
    m_pendingTodosListLabel = new QLabel(this);
    m_pendingTodosListLabel->setStyleSheet(TODO_HEADER_STYLE);
    m_pendingTodosHeaderLayout->addWidget(m_pendingTodosListLabel, 1);

    m_mainLayout->addWidget(headerContainer);
}

void BaseDashboardWidget::createTodoLists()
{
    // Create pending todos list with modern styling
    m_pendingTodosScrollArea = new QScrollArea(this);
    m_pendingTodosScrollArea->setWidgetResizable(true);
    m_pendingTodosScrollArea->setMaximumHeight(250);
    m_pendingTodosScrollArea->setStyleSheet(
        "QScrollArea {"
        "   background: transparent;"
        "   border: none;"
        "}"
        "QScrollBar:vertical {"
        "   background: rgba(255, 215, 215, 100);"
        "   width: 12px;"
        "   border-radius: 6px;"
        "   margin: 0px;"
        "}"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {"
        "   height: 0px;"
        "   width: 0px;"
        "   subcontrol-origin: margin;"
        "   subcontrol-position: bottom;"
        "}"
        "QScrollBar::handle:vertical {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "       stop:0 #FF6B7A, stop:1 #FF8A9A);"
        "   border-radius: 6px;"
        "   min-height: 20px;"
        "}"
        "QScrollBar::handle:vertical:hover {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "       stop:0 #FF5A6B, stop:1 #FF7A8B);"
        "}"
    );

    m_pendingTodosList = new QListWidget(this);
    m_pendingTodosScrollArea->setWidget(m_pendingTodosList);
    m_mainLayout->addWidget(m_pendingTodosScrollArea);

    // Create completed todos section
    m_completedTodosListLabel = new QLabel(this);
    m_completedTodosListLabel->setStyleSheet(COMPLETED_HEADER_STYLE);
    m_mainLayout->addWidget(m_completedTodosListLabel);

    // Create completed todos list
    m_completedTodosScrollArea = new QScrollArea(this);
    m_completedTodosScrollArea->setWidgetResizable(true);
    m_completedTodosScrollArea->setMaximumHeight(200);
    m_completedTodosScrollArea->setStyleSheet(
        "QScrollArea {"
        "   background: transparent;"
        "   border: none;"
        "}"
        "QScrollBar:vertical {"
        "   background: rgba(255, 215, 215, 100);"
        "   width: 12px;"
        "   border-radius: 6px;"
        "   margin: 0px;"
        "}"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {"
        "   height: 0px;"
        "   width: 0px;"
        "   subcontrol-origin: margin;"
        "   subcontrol-position: bottom;"
        "}"
        "QScrollBar::handle:vertical {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "       stop:0 #27ae60, stop:1 #2ecc71);"
        "   border-radius: 6px;"
        "   min-height: 20px;"
        "}"
        "QScrollBar::handle:vertical:hover {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "       stop:0 #229954, stop:1 #28b463);"
        "}"
    );

    m_completedTodosList = new QListWidget(this);
    m_completedTodosScrollArea->setWidget(m_completedTodosList);
    m_mainLayout->addWidget(m_completedTodosScrollArea);
}

void BaseDashboardWidget::connectSignals()
{
    // Connect todo list signals
    if (m_pendingTodosList) {
        connect(m_pendingTodosList, &QListWidget::itemDoubleClicked,
                this, &BaseDashboardWidget::onTodoItemDoubleClicked);

        m_pendingTodosList->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(m_pendingTodosList, &QListWidget::customContextMenuRequested,
                this, &BaseDashboardWidget::onTodoItemRightClicked);
    }

    if (m_completedTodosList) {
        connect(m_completedTodosList, &QListWidget::itemDoubleClicked,
                this, &BaseDashboardWidget::onTodoItemDoubleClicked);

        m_completedTodosList->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(m_completedTodosList, &QListWidget::customContextMenuRequested,
                this, &BaseDashboardWidget::onTodoItemRightClicked);
    }
}

void BaseDashboardWidget::updateTitle()
{
    if (m_isBeingDestroyed || !m_titleLabel) {
        return;
    }

    m_titleLabel->setText(getWidgetTitle());
}

void BaseDashboardWidget::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    if (!m_isBeingDestroyed) {
        QTimer::singleShot(100, this, [this]() {
            if (!m_isBeingDestroyed) {
                refreshFromDatabase();
            }
        });
    }
}

void BaseDashboardWidget::refreshFromDatabase()
{
    if (m_isBeingDestroyed) {
        return;
    }

    updateDateDisplay();
    calculateStats();
    updateTodosList();
    updateCounters();

    emit databaseUpdated();
}

void BaseDashboardWidget::updateDateDisplay()
{
    if (m_isBeingDestroyed) {
        return;
    }
    doUpdateDateDisplay();
}

void BaseDashboardWidget::calculateStats()
{
    if (m_isBeingDestroyed) {
        return;
    }

    // Get todos using virtual filter
    const auto allTodos = DatabaseManager::instance().getAllTodos(true);
    m_filteredTodos.clear();
    m_pendingTodosCount = 0;
    m_completedTodosCount = 0;

    for (const auto& todo : allTodos) {
        if (shouldIncludeTodo(todo)) {
            m_filteredTodos.append(todo);
            if (todo.isCompleted) {
                m_completedTodosCount++;
            } else {
                m_pendingTodosCount++;
            }
        }
    }

    doCalculateStats();
}

bool BaseDashboardWidget::shouldIncludeTodo(const DatabaseManager::TodoItem& todo) const
{
    return doShouldIncludeTodo(todo);
}

QString BaseDashboardWidget::getWidgetTitle() const
{
    return doGetWidgetTitle();
}

void BaseDashboardWidget::updateCounters()
{
    if (m_isBeingDestroyed || !m_statsLayout) {
        return;
    }

    // Clear existing counters
    QLayoutItem* item;
    while ((item = m_statsLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    // Create modern counter widgets with tomato theme
    CounterWidget* timeCounter = new CounterWidget(
        tr("Time Spent"), formatTime(m_timeSpentSeconds), "#FF6B7A", this);
    CounterWidget* todosCounter = new CounterWidget(
        tr("Pending To-dos"), QString::number(m_pendingTodosCount), "#FF8A9A", this);
    CounterWidget* completedCounter = new CounterWidget(
        tr("Completed To-dos"), QString::number(m_completedTodosCount), "#27ae60", this);
    CounterWidget* timersCounter = new CounterWidget(
        tr("Timers Completed"), QString::number(m_timersCompletedCount), "#3498db", this);

    // Add counters with proper spacing
    m_statsLayout->addWidget(timeCounter);
    m_statsLayout->addWidget(todosCounter);
    m_statsLayout->addWidget(completedCounter);
    m_statsLayout->addWidget(timersCounter);
}

QListWidgetItem* BaseDashboardWidget::createTodoItem(const DatabaseManager::TodoItem& todo, bool isCompleted)
{
    if (m_isBeingDestroyed) {
        return nullptr;
    }

    QString displayText = todo.title;
    if (displayText.length() > DashboardConstants::MAX_TODO_DISPLAY_LENGTH) {
        displayText = displayText.left(DashboardConstants::MAX_TODO_DISPLAY_LENGTH - 3) + "...";
    }

    QListWidgetItem* item = new QListWidgetItem();

    // Set item styling based on completion status
    if (isCompleted) {
        item->setIcon(QIcon(":/icons/check_circle.png"));
        item->setText(displayText);
        item->setToolTip(todo.title + "\n" + todo.description);

        // Completed item styling
        QFont font = item->font();
        font.setStrikeOut(true);
        item->setFont(font);

        item->setForeground(QColor("#7f8c8d"));
    } else {
        item->setIcon(QIcon(":/icons/circle.png"));
        item->setText(displayText);
        item->setToolTip(todo.title + "\n" + todo.description);

        // Priority-based styling for pending items
        QColor priorityColor;
        switch (todo.priority) {
            case DatabaseManager::TodoPriority::High:
                priorityColor = QColor("#e74c3c");
                break;
            case DatabaseManager::TodoPriority::Medium:
                priorityColor = QColor("#f39c12");
                break;
            case DatabaseManager::TodoPriority::Low:
                priorityColor = QColor("#27ae60");
                break;
        }

        item->setForeground(priorityColor);
    }

    // Store todo data
    item->setData(Qt::UserRole, todo.id);
    item->setData(Qt::UserRole + 1, todo.isCompleted);

    return item;
}

void BaseDashboardWidget::updateTodosList()
{
    if (m_isBeingDestroyed || !m_pendingTodosList || !m_completedTodosList) {
        return;
    }

    // Clear lists
    m_pendingTodosList->clear();
    m_completedTodosList->clear();

    // Add filtered todos to appropriate lists
    for (const auto& todo : m_filteredTodos) {
        QListWidgetItem* item = createTodoItem(todo, todo.isCompleted);
        if (item) {
            if (todo.isCompleted) {
                m_completedTodosList->addItem(item);
            } else {
                m_pendingTodosList->addItem(item);
            }
        }
    }

    // Update list visibility based on content
    m_completedTodosScrollArea->setVisible(true);
    m_completedTodosListLabel->setVisible(true);
}

void BaseDashboardWidget::onTodoItemDoubleClicked(QListWidgetItem* item)
{
    if (m_isBeingDestroyed || !item) {
        return;
    }

    int todoId = item->data(Qt::UserRole).toInt();
    if (todoId > 0) {
        emit todoEditRequested(todoId);
    }
}

void BaseDashboardWidget::onTodoItemRightClicked(const QPoint& pos)
{
    QListWidget* listWidget = qobject_cast<QListWidget*>(sender());
    if (!listWidget || m_isBeingDestroyed) {
        return;
    }

    QListWidgetItem* item = listWidget->itemAt(pos);
    if (!item) {
        return;
    }

    int todoId = item->data(Qt::UserRole).toInt();
    bool isCompleted = item->data(Qt::UserRole + 1).toBool();

    // Create modern context menu
    QMenu contextMenu(this);
    contextMenu.setStyleSheet(
        "QMenu {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "       stop:0 #FFFFFF, stop:1 #FFF8F8);"
        "   border: 2px solid #FFE0E0;"
        "   border-radius: 8px;"
        "   padding: 5px;"
        "}"
        "QMenu::item {"
        "   background: transparent;"
        "   padding: 8px 16px;"
        "   border-radius: 4px;"
        "   color: #2c3e50;"
        "}"
        "QMenu::item:selected {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "       stop:0 #FFE5E5, stop:1 #FFF0F0);"
        "   color: #2c3e50;"
        "}"
    );

    QAction* editAction = contextMenu.addAction(tr("Edit"));
    QAction* toggleAction = contextMenu.addAction(
        isCompleted ? tr("Mark as Pending") : tr("Mark as Completed"));
    QAction* deleteAction = contextMenu.addAction(tr("Delete"));
    deleteAction->setIcon(QIcon(":/icons/delete.png"));

    QAction* selectedAction = contextMenu.exec(listWidget->mapToGlobal(pos));

    if (selectedAction == editAction) {
        onTodoItemAction(todoId, "edit");
    } else if (selectedAction == toggleAction) {
        onTodoItemAction(todoId, "toggle");
    } else if (selectedAction == deleteAction) {
        onTodoItemAction(todoId, "delete");
    }
}

void BaseDashboardWidget::onTodoItemAction(int todoId, const QString& action)
{
    if (m_isBeingDestroyed) {
        return;
    }

    if (action == "edit") {
        emit todoEditRequested(todoId);
    } else if (action == "toggle") {
        DatabaseManager::TodoItem todo = DatabaseManager::instance().getTodo(todoId);
        onTodoToggled(todoId, !todo.isCompleted);
    } else if (action == "delete") {
        DatabaseManager::instance().deleteTodo(todoId);
        refreshFromDatabase();
    }
}

void BaseDashboardWidget::onTodoToggled(int todoId, bool completed)
{
    if (m_isBeingDestroyed) {
        return;
    }

    DatabaseManager::instance().toggleTodoCompletion(todoId, completed);
    refreshFromDatabase();
}

void BaseDashboardWidget::onAddTodoClicked()
{
    if (m_isBeingDestroyed) {
        return;
    }
    emit addTodoRequested();
}

QString BaseDashboardWidget::formatTime(int seconds) const
{
    int hours = seconds / 3600;
    int minutes = (seconds % 3600) / 60;

    if (hours > 0) {
        return QString("%1h %2m").arg(hours).arg(minutes);
    } else if (minutes > 0) {
        return QString("%1m").arg(minutes);
    } else {
        return QString("%1s").arg(seconds);
    }
}