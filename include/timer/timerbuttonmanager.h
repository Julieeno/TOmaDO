#ifndef TIMERBUTTONMANAGER_H
#define TIMERBUTTONMANAGER_H

#include <QObject>
#include <QPushButton>
#include <QWidget>
#include <map>
#include <memory>

class TimerButtonManager : public QObject {
    Q_OBJECT
    
public:
    enum class ButtonType {
        Start,
        Pause,
        Stop,
        EndBreak,
        Expand,
        Collapse
    };
    
    enum class ViewMode {
        Compact,
        Expanded
    };
    
    explicit TimerButtonManager(QWidget* parent = nullptr);
    
    QPushButton* createButton(ButtonType type, ViewMode mode);
    
    void updateButtonStates(bool isRunning, bool isPaused, bool isExpanded, bool isBreakTime = false);
    
    QPushButton* getButton(ButtonType type, ViewMode mode);
    
    void connectButtonSignals(ButtonType type, ViewMode mode, QObject* receiver, const char* slot);
    
private:
    QWidget* m_parent;
    
    std::map<std::pair<ButtonType, ViewMode>, QPushButton*> m_buttons;
    
    // Common button styling
    void applyButtonStyle(QPushButton* button, ButtonType type, ViewMode mode);
    QString getButtonStyleSheet(ButtonType type, ViewMode mode);
    QString getButtonText(ButtonType type, bool isPaused = false);
    void setButtonSize(QPushButton* button, ButtonType type, ViewMode mode);
};

#endif // TIMERBUTTONMANAGER_H