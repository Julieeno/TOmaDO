#ifndef GOALDIALOGUI_H
#define GOALDIALOGUI_H

#include <QWidget>

class QVBoxLayout;
class QFormLayout;
class QLineEdit;
class QTextEdit;
class QComboBox;
class QSpinBox;
class QCheckBox;
class QGroupBox;
class QDialogButtonBox;
class ColorPickerWidget;

class GoalDialogUI {
public:
    struct FormWidgets {
        QLineEdit* titleEdit;
        QTextEdit* descriptionEdit;
        QComboBox* priorityCombo;
        QSpinBox* timeSpinBox;
        QCheckBox* completionCheckbox;
        ColorPickerWidget* colorPicker;
        QDialogButtonBox* buttonBox;
    };
    
    static FormWidgets createUI(QWidget* parent);
    
private:
    static QVBoxLayout* createMainLayout(QWidget* parent);
    static QFormLayout* createFormLayout(QWidget* parent, FormWidgets& widgets);
    static QGroupBox* createDetailsGroup(QWidget* parent, FormWidgets& widgets);
    static QGroupBox* createColorGroup(QWidget* parent, FormWidgets& widgets);
    static QDialogButtonBox* createButtonBox(QWidget* parent);
    
    static void setupFormWidgets(FormWidgets& widgets);
    static void applyWidgetStyles(const FormWidgets& widgets);
    
    GoalDialogUI() = delete; // Static utility class
};

#endif // GOALDIALOGUI_H