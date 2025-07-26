#ifndef CALENDARNAVIGATION_H
#define CALENDARNAVIGATION_H

#include <QWidget>
#include <QDate>

class QHBoxLayout;
class QPushButton;

class CalendarNavigation : public QWidget {
    Q_OBJECT

public:
    explicit CalendarNavigation(QWidget* parent = nullptr);

    void updateDisplay(const QDate& date);

    signals:
        void previousMonthClicked();
    void nextMonthClicked();
    void todayClicked();
    void monthSelected(int month);
    void yearSelected(int year);

private slots:
    void onMonthButtonClicked();
    void onYearButtonClicked();

private:
    void setupUi();
    void createButtons();
    void setupLayout();
    void connectSignals();

    QHBoxLayout* m_layout;
    QPushButton* m_previousButton;
    QPushButton* m_nextButton;
    QPushButton* m_todayButton;
    QPushButton* m_monthButton;
    QPushButton* m_yearButton;
};

#endif // CALENDARNAVIGATION_H