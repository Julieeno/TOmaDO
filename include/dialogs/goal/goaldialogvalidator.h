#ifndef GOALDIALOGVALIDATOR_H
#define GOALDIALOGVALIDATOR_H

#include <QString>
#include <QWidget>

struct GoalValidationResult {
    bool isValid;
    QString errorMessage;
    QWidget* focusWidget;
    
    GoalValidationResult(bool valid = true, const QString& message = QString(), QWidget* widget = nullptr)
        : isValid(valid), errorMessage(message), focusWidget(widget) {}
};

class GoalDialogValidator {
public:
    static GoalValidationResult validateTitle(const QString& title, QWidget* titleWidget);
    static GoalValidationResult validateDescription(const QString& description, QWidget* descriptionWidget);
    static GoalValidationResult validateEstimatedTime(int minutes, QWidget* timeWidget);
    static GoalValidationResult validateAll(const QString& title, const QString& description, 
                                          int estimatedTime, QWidget* titleWidget, 
                                          QWidget* descriptionWidget, QWidget* timeWidget);
    
private:
    static bool isValidTitle(const QString& title);
    static bool isValidDescription(const QString& description);
    static bool isValidEstimatedTime(int minutes);
    
    GoalDialogValidator() = delete; // Static utility class
};

#endif // GOALDIALOGVALIDATOR_H