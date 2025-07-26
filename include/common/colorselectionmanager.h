#ifndef COLORSELECTIONMANAGER_H
#define COLORSELECTIONMANAGER_H

#include <QObject>
#include <QString>
#include <QList>

class ColorButton;
class QButtonGroup;

class ColorSelectionManager : public QObject {
    Q_OBJECT

public:
    explicit ColorSelectionManager(QObject* parent = nullptr);

    void setButtons(const QList<ColorButton*>& buttons);
    void setButtonGroup(QButtonGroup* buttonGroup);

    void setSelectedColor(const QString& colorCode);
    QString selectedColor() const { return m_selectedColor; }

    signals:
        void colorSelected(const QString& colorCode);

private slots:
    void onColorButtonClicked();

private:
    void updateButtonSelection(const QString& colorCode);
    ColorButton* findButtonByColor(const QString& colorCode) const;

    QList<ColorButton*> m_colorButtons;
    QButtonGroup* m_buttonGroup;
    QString m_selectedColor;
};

#endif // COLORSELECTIONMANAGER_H