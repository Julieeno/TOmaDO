#ifndef DIALOGSTYLES_H
#define DIALOGSTYLES_H

#include <QString>
#include <QDialogButtonBox>
#include <QIcon>
#include <QPushButton>

/**
 * @brief Centralized dialog styling for consistent UI across the application
 */
class DialogStyles {
public:
    /**
     * @brief Configure a dialog button box to remove icons and apply consistent styling
     * @param buttonBox The QDialogButtonBox to configure
     */
    static void configureButtonBox(QDialogButtonBox* buttonBox) {
        // Remove icons from all buttons
        if (auto* okButton = buttonBox->button(QDialogButtonBox::Ok)) {
            okButton->setIcon(QIcon());
            okButton->setAttribute(Qt::WA_Hover, true);
        }
        if (auto* cancelButton = buttonBox->button(QDialogButtonBox::Cancel)) {
            cancelButton->setIcon(QIcon());
            cancelButton->setAttribute(Qt::WA_Hover, true);

            // Apply custom styling to Cancel button
            cancelButton->setStyleSheet(
                "QPushButton {"
                "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
                "       stop:0 #bdc3c7, stop:1 #95a5a6);"
                "   color: white;"
                "   border-radius: 8px;"
                "   padding: 12px 24px;"
                "   font-weight: 600;"
                "   font-size: 14px;"
                "   min-width: 80px;"
                "   margin: 0 4px;"
                "}"
                "QPushButton:hover {"
                "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
                "       stop:0 #95a5a6, stop:1 #7f8c8d);"
                "}"
                "QPushButton:pressed {"
                "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
                "       stop:0 #95a5a6, stop:1 #7f8c8d);"
                "}"
            );
        }
        if (auto* saveButton = buttonBox->button(QDialogButtonBox::Save)) {
            saveButton->setIcon(QIcon());
            saveButton->setAttribute(Qt::WA_Hover, true);
        }
        if (auto* applyButton = buttonBox->button(QDialogButtonBox::Apply)) {
            applyButton->setIcon(QIcon());
            applyButton->setAttribute(Qt::WA_Hover, true);
        }
    }

    /**
     * @brief Get the standard dialog button box stylesheet with consistent pastel red styling
     * @return CSS stylesheet for QDialogButtonBox
     */
    static QString getDialogButtonBoxStyle() {
        return
            "QDialogButtonBox {"
            "   background: transparent;"
            "   border: none;"
            "   padding: 10px 0;"
            "}"
            "QDialogButtonBox QPushButton {"
            "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
            "       stop:0 #FF6B7A, stop:1 #FF5A6B);"
            "   color: white;"
            "   border: none;"
            "   border-radius: 8px;"
            "   padding: 12px 24px;"
            "   font-weight: 600;"
            "   font-size: 14px;"
            "   min-width: 80px;"
            "   margin: 0 4px;"
            "   text-align: center;"
            "   icon: none;"
            "}"
            "QDialogButtonBox QPushButton:hover {"
            "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
            "       stop:0 #FF5A6B, stop:1 #FF4D5E);"
            "   transform: translateY(-1px);"
            "}"
            "QDialogButtonBox QPushButton:pressed {"
            "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
            "       stop:0 #FF4D5E, stop:1 #FF3E51);"
            "   transform: translateY(1px);"
            "}"
            "QDialogButtonBox QPushButton:default {"
            "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
            "       stop:0 #FF6B7A, stop:1 #FF5A6B);"
            "   border: 2px solid #FF5A6B;"
            "}"
            "QDialogButtonBox QPushButton:default:hover {"
            "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
            "       stop:0 #FF5A6B, stop:1 #FF4D5E);"
            "   border: 2px solid #FF4D5E;"
            "}"
            "QDialogButtonBox QPushButton:disabled {"
            "   background: #CCCCCC;"
            "   color: #999999;"
            "}"
            "QDialogButtonBox QPushButton:disabled:hover {"
            "   background: #CCCCCC;"
            "}";
    }

    /**
     * @brief Get styling for standalone buttons (non-dialog button box)
     * @return CSS stylesheet for standalone QPushButton
     */
    static QString getStandaloneButtonStyle() {
        return
            "QPushButton {"
            "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
            "       stop:0 #FF6B7A, stop:1 #FF5A6B);"
            "   color: white;"
            "   border: none;"
            "   border-radius: 8px;"
            "   padding: 12px 24px;"
            "   font-weight: 600;"
            "   font-size: 14px;"
            "   min-width: 80px;"
            "   margin: 0 4px;"
            "   text-align: center;"
            "}"
            "QPushButton:hover {"
            "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
            "       stop:0 #FF5A6B, stop:1 #FF4D5E);"
            "}"
            "QPushButton:pressed {"
            "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
            "       stop:0 #FF4D5E, stop:1 #FF3E51);"
            "}"
            "QPushButton:disabled {"
            "   background: #CCCCCC;"
            "   color: #999999;"
            "}";
    }

    /**
     * @brief Configure a standalone button with consistent styling
     * @param button The QPushButton to configure
     */
    static void configureStandaloneButton(QPushButton* button) {
        if (button) {
            button->setIcon(QIcon());
            button->setAttribute(Qt::WA_Hover, true);
            button->setStyleSheet(getStandaloneButtonStyle());
        }
    }

    // Rest of the getDialogStyle() method remains the same...
    static QString getDialogStyle() {
        return
            "QDialog {"
            "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
            "       stop:0 #FFFBFB, stop:0.5 #FFF5F5, stop:1 #FFEFEF);"
            "   border-radius: 12px;"
            "}"
            "QLabel {"
            "   color: #2c3e50;"
            "   font-weight: 600;"
            "   font-size: 14px;"
            "   background: transparent;"
            "}"
            "QLineEdit, QTextEdit {"
            "   background: rgba(255, 255, 255, 180);"
            "   border: 2px solid #FFE0E0;"
            "   border-radius: 8px;"
            "   padding: 8px 12px;"
            "   font-size: 14px;"
            "   color: #2c3e50;"
            "}"
            "QLineEdit:focus, QTextEdit:focus {"
            "   border: 2px solid #FF6B7A;"
            "   background: rgba(255, 255, 255, 220);"
            "}"
            "QLineEdit:hover, QTextEdit:hover {"
            "   border: 2px solid #FF8A9A;"
            "   background: rgba(255, 255, 255, 200);"
            "}"
            "QComboBox {"
            "   background: rgba(255, 255, 255, 180);"
            "   border: 2px solid #FFE0E0;"
            "   border-radius: 8px;"
            "   padding: 8px 12px;"
            "   font-size: 14px;"
            "   color: #2c3e50;"
            "}"
            "QComboBox:hover {"
            "   border: 2px solid #FF8A9A;"
            "   background: rgba(255, 255, 255, 200);"
            "}"
            "QComboBox:focus {"
            "   border: 2px solid #FF6B7A;"
            "   background: rgba(255, 255, 255, 220);"
            "}"
            "QCheckBox {"
            "   color: #2c3e50;"
            "   font-weight: 500;"
            "   font-size: 14px;"
            "   spacing: 8px;"
            "}"
            "QCheckBox::indicator {"
            "   width: 18px;"
            "   height: 18px;"
            "   border: 2px solid #FFE0E0;"
            "   border-radius: 4px;"
            "   background: rgba(255, 255, 255, 180);"
            "}"
            "QCheckBox::indicator:hover {"
            "   border: 2px solid #FF6B7A;"
            "   background: rgba(255, 255, 255, 220);"
            "}"
            "QCheckBox::indicator:checked {"
            "   background: #FF6B7A;"
            "   border: 2px solid #FF6B7A;"
            "}"
            "QCheckBox::indicator:checked:hover {"
            "   background: #FF5A6B;"
            "   border: 2px solid #FF5A6B;"
            "}"
            "QGroupBox {"
            "   font-weight: 600;"
            "   font-size: 16px;"
            "   color: #2c3e50;"
            "   border: 2px solid #FFE0E0;"
            "   border-radius: 12px;"
            "   margin-top: 12px;"
            "   padding-top: 15px;"
            "   background: rgba(255, 255, 255, 80);"
            "}"
            "QGroupBox::title {"
            "   subcontrol-origin: margin;"
            "   left: 15px;"
            "   padding: 5px 12px;"
            "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
            "       stop:0 #FFE5E5, stop:1 #FFF0F0);"
            "   border-radius: 6px;"
            "   border: 1px solid #FFD6D6;"
            "}"
            "QScrollArea {"
            "   border: none;"
            "   background: transparent;"
            "}"
            "QSpinBox {"
            "   background: rgba(255, 255, 255, 180);"
            "   border: 2px solid #FFE0E0;"
            "   border-radius: 8px;"
            "   padding: 8px 12px;"
            "   font-size: 14px;"
            "   color: #2c3e50;"
            "}"
            "QSpinBox:hover {"
            "   border: 2px solid #FF8A9A;"
            "   background: rgba(255, 255, 255, 200);"
            "}"
            "QSpinBox:focus {"
            "   border: 2px solid #FF6B7A;"
            "   background: rgba(255, 255, 255, 220);"
            "}"
            + getDialogButtonBoxStyle();
    }
};

#endif // DIALOGSTYLES_H