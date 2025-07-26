#ifndef TODODIALOGVALIDATOR_H
#define TODODIALOGVALIDATOR_H

#include <QString>
#include <QDateTime>

class QWidget;

class TodoDialogValidator {
public:
    struct ValidationResult {
        bool isValid = true;
        QString errorMessage;
        QWidget* focusWidget = nullptr;
    };
    
    static ValidationResult validateTitle(const QString& title, QWidget* titleWidget);
    static ValidationResult validateDates(const QDateTime& startDate, const QDateTime& endDate,
                                        bool hasStartDate, bool hasEndDate,
                                        QWidget* startWidget, QWidget* endWidget);

    static ValidationResult validateAll(const QString& title, const QDateTime& startDate, 
                                       const QDateTime& endDate, bool hasStartDate, 
                                       bool hasEndDate, QWidget* titleWidget,
                                       QWidget* startWidget, QWidget* endWidget,
                                       QWidget* timeWidget);
    
private:
    TodoDialogValidator() = delete; // Static utility class
};

#endif // TODODIALOGVALIDATOR_H