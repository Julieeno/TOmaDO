#ifndef COLORPICKERWIDGET_H
#define COLORPICKERWIDGET_H

#include <QWidget>
#include <QString>
#include <QList>

class QGridLayout;
class QButtonGroup;
class ColorButton;
class ColorSelectionManager;

class ColorPickerWidget : public QWidget {
    Q_OBJECT

public:
    explicit ColorPickerWidget(QWidget* parent = nullptr);
    ~ColorPickerWidget() override;

    void setSelectedColor(const QString& colorCode);
    QString selectedColor() const;

    signals:
        void colorSelected(const QString& colorCode);

private:
    void setupUi();
    void setupStyling();
    void setupLayout();
    void createColorButtons();
    void configureWidget();
    void connectSignals();

    // UI Components
    QGridLayout* m_gridLayout;
    QButtonGroup* m_buttonGroup;
    QList<ColorButton*> m_colorButtons;

    // Managers
    ColorSelectionManager* m_selectionManager;

    // Data
    QList<QString> m_availableColors;
};

#endif // COLORPICKERWIDGET_H