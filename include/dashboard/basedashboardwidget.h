#ifndef BASEDASHBOARDWIDGET_H
#define BASEDASHBOARDWIDGET_H

#include <QDate>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QScrollArea>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

#include "database/databasemanager.h"

class BaseDashboardWidget : public QWidget {
    Q_OBJECT

public:
    explicit BaseDashboardWidget(QWidget* parent = nullptr);

    ~BaseDashboardWidget();

    // Public interface methods
    void refreshFromDatabase();

    void onAddTodoClicked();

    void updateTitle();

signals:
    void databaseUpdated();

    void addTodoRequested();

    void todoEditRequested(int todoId);

protected:
    // UI Constants
    static constexpr int REFRESH_INTERVAL_MS = 60000;  // 1 minute
    static constexpr int DEFAULT_PADDING = 15;
    static constexpr int BORDER_RADIUS = 10;

    // String constants
    static const QString TODAY_PENDING_TODOS_TITLE;
    static const QString TODAY_COMPLETED_TODOS_TITLE;
    static const QString WEEK_PENDING_TODOS_TITLE;
    static const QString WEEK_COMPLETED_TODOS_TITLE;
    static const QString TOTAL_PENDING_TODOS_TITLE;
    static const QString TOTAL_COMPLETED_TODOS_TITLE;

    // Destruction flag to prevent virtual calls in destructor
    bool m_isBeingDestroyed;

    // Core UI components
    QVBoxLayout* m_mainLayout;
    QLabel* m_titleLabel;
    QLabel* m_dateLabel;

    // Statistics counter components
    struct CounterSet {
        QFrame* frame = nullptr;
        QLabel* label = nullptr;
        QLabel* value = nullptr;
    };

    QWidget* m_statsWidget;
    QHBoxLayout* m_statsLayout;
    CounterSet m_timeSpent;
    CounterSet m_timersCompleted;
    CounterSet m_pendingTodos;
    CounterSet m_completedTodos;

    // Todo list components
    QHBoxLayout* m_pendingTodosHeaderLayout;
    QLabel* m_pendingTodosListLabel;
    QScrollArea* m_pendingTodosScrollArea;
    QListWidget* m_pendingTodosList;

    QLabel* m_completedTodosListLabel;
    QScrollArea* m_completedTodosScrollArea;
    QListWidget* m_completedTodosList;

    // Timer for periodic refresh
    QTimer* m_refreshTimer;

    // Statistics data
    int m_pendingTodosCount;
    int m_completedTodosCount;
    int m_timeSpentSeconds;
    int m_timersCompletedCount;
    QList<DatabaseManager::TodoItem> m_filteredTodos;

    // Style constants
    static const QString TITLE_STYLE;
    static const QString DATE_STYLE;
    static const QString COUNTER_FRAME_STYLE;
    static const QString COUNTER_LABEL_STYLE;
    static const QString COUNTER_VALUE_STYLE;
    static const QString TODO_HEADER_STYLE;
    static const QString COMPLETED_HEADER_STYLE;

    // UI setup methods

    virtual void setupUi();

    void setupCounters();

    void createTodoLists();

    void createTodoHeader();

    void connectSignals();

    void showEvent(QShowEvent *event) override;

    // Helper methods
    QListWidgetItem* createTodoItem(const DatabaseManager::TodoItem& todo, bool isCompleted);

    QString formatTime(int seconds) const;

    // Event handlers
    void onTodoItemDoubleClicked(QListWidgetItem* item);

    void onTodoItemRightClicked(const QPoint& pos);

    void onTodoItemAction(int todoId, const QString& action);

    void onTodoToggled(int todoId, bool completed);

    // Abstract interface for subclasses
    virtual void doUpdateDateDisplay() = 0;

    virtual void doCalculateStats() = 0;

    virtual bool doShouldIncludeTodo(const DatabaseManager::TodoItem& todo) const = 0;

    virtual QString doGetWidgetTitle() const = 0;

private:
    // Non-virtual interface methods
    void updateDateDisplay();
    void calculateStats();
    bool shouldIncludeTodo(const DatabaseManager::TodoItem& todo) const;
    QString getWidgetTitle() const;
    void updateCounters();
    void updateTodosList();
};

#endif // BASEDASHBOARDWIDGET_H