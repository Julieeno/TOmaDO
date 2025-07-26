#include "dialogs/todo/tododialogvalidator.h"
#include "dialogs/todo/tododialogconstants.h"
#include <QWidget>

TodoDialogValidator::ValidationResult TodoDialogValidator::validateTitle(const QString& title, QWidget* titleWidget)
{
    ValidationResult result;
    
    if (title.trimmed().isEmpty()) {
        result.isValid = false;
        result.errorMessage = TodoDialogConstants::TITLE_REQUIRED_MESSAGE;
        result.focusWidget = titleWidget;
    }
    
    return result;
}

TodoDialogValidator::ValidationResult TodoDialogValidator::validateDates(const QDateTime& startDate, const QDateTime& endDate,
                                                                       bool hasStartDate, bool hasEndDate,
                                                                       QWidget* startWidget, QWidget* endWidget)
{
    ValidationResult result;
    
    // Only validate if both dates are enabled
    if (hasStartDate && hasEndDate) {
        if (startDate >= endDate) {
            result.isValid = false;
            result.errorMessage = TodoDialogConstants::DATE_ERROR_MESSAGE;
            result.focusWidget = endWidget;
        }
    }
    
    return result;
}

TodoDialogValidator::ValidationResult TodoDialogValidator::validateAll(const QString& title, const QDateTime& startDate, 
                                                                     const QDateTime& endDate, bool hasStartDate, 
                                                                     bool hasEndDate, QWidget* titleWidget,
                                                                     QWidget* startWidget, QWidget* endWidget,
                                                                     QWidget* timeWidget)
{
    // Validate title first
    auto titleResult = validateTitle(title, titleWidget);
    if (!titleResult.isValid) {
        return titleResult;
    }
    
    // Validate dates
    auto dateResult = validateDates(startDate, endDate, hasStartDate, hasEndDate, startWidget, endWidget);
    if (!dateResult.isValid) {
        return dateResult;
    }

    return ValidationResult(); // All validations passed
}