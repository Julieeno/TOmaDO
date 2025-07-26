#include "timer/timerbuttonmanager.h"
#include <QApplication>

TimerButtonManager::TimerButtonManager(QWidget* parent)
    : QObject(parent), m_parent(parent) {}


QPushButton* TimerButtonManager::createButton(ButtonType type, ViewMode mode) {
    auto key = std::make_pair(type, mode);

    // Return existing button if already created
    if (m_buttons.find(key) != m_buttons.end()) {
        return m_buttons[key];
    }

    // Create new button
    QPushButton* button = new QPushButton(m_parent);

    // Set button text based on mode
    if (mode == ViewMode::Expanded) {
        switch (type) {
            case ButtonType::Start:
                button->setText("▶ Start");
                break;
            case ButtonType::Pause:
                button->setText("⏸ Pause");
                break;
            case ButtonType::Stop:
                button->setText("⏹ Stop");
                break;
            case ButtonType::EndBreak:
                button->setText("⏹ End Break");
                break;
            default:
                button->setText(getButtonText(type));
                break;
        }
    } else {
        button->setText(getButtonText(type));
    }

    // Set button size
    setButtonSize(button, type, mode);

    // Apply styling
    applyButtonStyle(button, type, mode);

    // Store reference
    m_buttons[key] = button;

    return button;
}

void TimerButtonManager::updateButtonStates(bool isRunning, bool isPaused, bool isExpanded, bool isBreakTime) {
    // Update all buttons of each type
    for (auto& pair : m_buttons) {
        ButtonType type = pair.first.first;
        ViewMode mode = pair.first.second;
        QPushButton* button = pair.second;

        if (isBreakTime) {
            switch (type) {
                case ButtonType::Start:
                    button->setEnabled(false);
                    button->setVisible(false);
                    break;

                case ButtonType::Pause:
                    button->setEnabled(false);
                    button->setVisible(false);
                    break;

                case ButtonType::Stop:
                    button->setEnabled(false);
                    button->setVisible(false);
                    break;

                case ButtonType::EndBreak:
                    button->setEnabled(true);
                    button->setVisible(true);
                    break;

                case ButtonType::Expand:
                    button->setVisible(!isExpanded);
                    break;

                case ButtonType::Collapse:
                    button->setVisible(isExpanded);
                    break;
            }
        } else {
            switch (type) {
                case ButtonType::Start:
                    button->setEnabled(!isRunning && !isPaused);
                    button->setVisible(!isRunning && !isPaused);
                    break;

                case ButtonType::Pause:
                    // The pause button should be enabled when running OR when paused (to resume)
                    button->setEnabled(isRunning || isPaused);
                    button->setVisible(isRunning || isPaused);
                    // Update text based on current state and mode
                    if (mode == ViewMode::Expanded) {
                        button->setText(isPaused ? "▶ Resume" : "⏸ Pause");
                    } else {
                        button->setText(isPaused ? "▶" : "⏸");
                    }
                    break;

                case ButtonType::Stop:
                    button->setEnabled(isRunning || isPaused);
                    button->setVisible(isRunning || isPaused);
                    break;

                case ButtonType::EndBreak:
                    button->setEnabled(false);
                    button->setVisible(false);
                    break;

                case ButtonType::Expand:
                    button->setVisible(!isExpanded);
                    break;

                case ButtonType::Collapse:
                    button->setVisible(isExpanded);
                    break;
            }
        }
    }
}

QString TimerButtonManager::getButtonText(ButtonType type, bool isPaused) {
    // For now, return the text - the createButton method will need to know the mode
    switch (type) {
        case ButtonType::Start:
            return "▶";
        case ButtonType::Pause:
            return isPaused ? "▶" : "⏸";
        case ButtonType::Stop:
            return "⏹";
        case ButtonType::EndBreak:
            return "⏹";
        case ButtonType::Expand:
            return "⤢";
        case ButtonType::Collapse:
            return "Collapse";
    }
    return "";
}

QString TimerButtonManager::getButtonStyleSheet(ButtonType type, ViewMode mode) {
    if (mode == ViewMode::Compact) {
        switch (type) {
            case ButtonType::Start:
                return
                    "QPushButton {"
                    "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
                    "       stop:0 #2ecc71, stop:1 #27ae60);"
                    "   color: white;"
                    "   border: 2px solid #27ae60;"
                    "   border-radius: 23px;"
                    "   font-size: 18px;"
                    "   font-weight: bold;"
                    "}"
                    "QPushButton:hover {"
                    "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
                    "       stop:0 #58d68d, stop:1 #2ecc71);"
                    "   border: 2px solid #2ecc71;"
                    "}"
                    "QPushButton:pressed {"
                    "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
                    "       stop:0 #27ae60, stop:1 #229954);"
                    "   border: 2px solid #1e8449;"
                    "}";

            case ButtonType::Pause:
                return
                    "QPushButton {"
                    "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
                    "       stop:0 #f39c12, stop:1 #e67e22);"
                    "   color: white;"
                    "   border: 2px solid #e67e22;"
                    "   border-radius: 23px;"
                    "   font-size: 18px;"
                    "   font-weight: bold;"
                    "}"
                    "QPushButton:hover {"
                    "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
                    "       stop:0 #f7b731, stop:1 #f39c12);"
                    "   border: 2px solid #f39c12;"
                    "}";

            case ButtonType::EndBreak:
                return
                    "QPushButton {"
                    "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
                    "       stop:0 #FFA500, stop:1 #FF8C00);"
                    "   color: white;"
                    "   border: 2px solid #FF8C00;"
                    "   border-radius: 23px;"
                    "   font-size: 18px;"
                    "   font-weight: bold;"
                    "}"
                    "QPushButton:hover {"
                    "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
                    "       stop:0 #FFB347, stop:1 #FFA500);"
                    "   border: 2px solid #FFA500;"
                    "}";

            case ButtonType::Expand:
                return
                    "QPushButton {"
                    "   background-color: rgba(255, 255, 255, 200);"
                    "   color: #6c757d;"
                    "   border: 1px solid #dee2e6;"
                    "   border-radius: 10px;"
                    "   font-size: 11px;"
                    "   font-weight: bold;"
                    "}"
                    "QPushButton:hover {"
                    "   background-color: #ffffff;"
                    "   color: #3498db;"
                    "   border: 1px solid #3498db;"
                    "}"
                    "QPushButton:pressed {"
                    "   background-color: #e3f2fd;"
                    "   color: #2980b9;"
                    "}";

            default:
                return "";
        }
    }
    else { // Expanded mode
        switch (type) {
            case ButtonType::Start:
                return
                    "QPushButton {"
                    "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
                    "       stop:0 #2ecc71, stop:1 #27ae60);"
                    "   color: white;"
                    "   border: 2px solid #27ae60;"
                    "   border-radius: 16px;"
                    "   font-size: 16px;"
                    "   font-weight: 600;"
                    "}"
                    "QPushButton:hover {"
                    "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
                    "       stop:0 #58d68d, stop:1 #2ecc71);"
                    "   border: 2px solid #2ecc71;"
                    "}"
                    "QPushButton:pressed {"
                    "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
                    "       stop:0 #27ae60, stop:1 #229954);"
                    "   border: 2px solid #1e8449;"
                    "}";

            case ButtonType::Pause:
                return
                    "QPushButton {"
                    "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
                    "       stop:0 #f39c12, stop:0.3 #e67e22, stop:0.7 #d35400, stop:1 #c0392b);"
                    "   color: white;"
                    "   border: 2px solid #c0392b;"
                    "   border-radius: 16px;"
                    "   font-size: 16px;"
                    "   font-weight: 600;"
                    "}"
                    "QPushButton:hover {"
                    "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
                    "       stop:0 #f7b731, stop:0.3 #f39c12, stop:0.7 #e67e22, stop:1 #d35400);"
                    "   border: 2px solid #e67e22;"
                    "}";

            case ButtonType::Stop:
                return
                    "QPushButton {"
                    "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
                    "       stop:0 #e74c3c, stop:1 #c0392b);"
                    "   color: white;"
                    "   border: 2px solid #c0392b;"
                    "   border-radius: 16px;"
                    "   font-size: 16px;"
                    "   font-weight: 600;"
                    "}"
                    "QPushButton:hover {"
                    "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
                    "       stop:0 #ec7063, stop:1 #e74c3c);"
                    "   border: 2px solid #e74c3c;"
                    "}"
                    "QPushButton:pressed {"
                    "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
                    "       stop:0 #FFF0F0, stop:0.5 #FFE8E8, stop:1 #FFE0E0);"
                    "}";

            case ButtonType::EndBreak:
                return
                    "QPushButton {"
                    "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
                    "       stop:0 #FFA500, stop:0.3 #FF8C00, stop:0.7 #FF7F00, stop:1 #FF6600);"
                    "   color: white;"
                    "   border: 2px solid #FF6600;"
                    "   border-radius: 16px;"
                    "   font-size: 16px;"
                    "   font-weight: 600;"
                    "}"
                    "QPushButton:hover {"
                    "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
                    "       stop:0 #FFB347, stop:0.3 #FFA500, stop:0.7 #FF8C00, stop:1 #FF7F00);"
                    "   border: 2px solid #FF8C00;"
                    "}";

            case ButtonType::Collapse:
                return
                    "QPushButton {"
                    "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
                    "       stop:0 #FFFFFF, stop:1 #FFF8F8);"
                    "   color: #8B4B6B;"
                    "   border: 2px solid #FFD6D6;"
                    "   border-radius: 8px;"
                    "   font-size: 12px;"
                    "   font-weight: 500;"
                    "}"
                    "QPushButton:hover {"
                    "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
                    "       stop:0 #FFF8F8, stop:1 #FFF0F0);"
                    "   border: 2px solid #FFB6C1;"
                    "   color: #8B2A5B;"
                    "}";

            default:
                return "";
        }
    }
}

QPushButton* TimerButtonManager::getButton(ButtonType type, ViewMode mode) {
    auto key = std::make_pair(type, mode);
    auto it = m_buttons.find(key);
    return (it != m_buttons.end()) ? it->second : nullptr;
}

void TimerButtonManager::connectButtonSignals(ButtonType type, ViewMode mode, QObject* receiver, const char* slot) {
    QPushButton* button = getButton(type, mode);
    if (button) {
        QObject::connect(button, SIGNAL(clicked()), receiver, slot);
    }
}

void TimerButtonManager::applyButtonStyle(QPushButton* button, ButtonType type, ViewMode mode) {
    button->setStyleSheet(getButtonStyleSheet(type, mode));
}

void TimerButtonManager::setButtonSize(QPushButton* button, ButtonType type, ViewMode mode) {
    if (mode == ViewMode::Compact) {
        switch (type) {
            case ButtonType::Expand:
                button->setFixedSize(20, 20);
                break;
            case ButtonType::Start:
            case ButtonType::Pause:
            case ButtonType::EndBreak:
                button->setFixedSize(46, 46);
                break;
            default:
                button->setFixedSize(46, 46);
                break;
        }
    } else { // Expanded
        switch (type) {
            case ButtonType::Collapse:
                button->setFixedSize(100, 30);
                break;
            case ButtonType::Start:
            case ButtonType::Pause:
            case ButtonType::Stop:
            case ButtonType::EndBreak:
                button->setFixedSize(120, 50);
                break;
            default:
                button->setFixedSize(120, 50);
                break;
        }
    }
}