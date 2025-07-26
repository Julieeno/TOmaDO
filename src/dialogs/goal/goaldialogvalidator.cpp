#include "dialogs/goal/goaldialogvalidator.h"
#include "dialogs/goal/goaldialogconstants.h"

GoalValidationResult GoalDialogValidator::validateTitle(const QString& title, QWidget* titleWidget)
{
    if (!isValidTitle(title)) {
        return GoalValidationResult(false, GoalDialogConstants::TITLE_REQUIRED_MESSAGE, titleWidget);
    }
    return GoalValidationResult(true);
}

GoalValidationResult GoalDialogValidator::validateDescription(const QString& description, QWidget* descriptionWidget)
{
    if (!isValidDescription(description)) {
        return GoalValidationResult(false, "Description is too long", descriptionWidget);
    }
    return GoalValidationResult(true);
}

GoalValidationResult GoalDialogValidator::validateEstimatedTime(int minutes, QWidget* timeWidget)
{
    if (!isValidEstimatedTime(minutes)) {
        return GoalValidationResult(false, GoalDialogConstants::INVALID_TIME_MESSAGE, timeWidget);
    }
    return GoalValidationResult(true);
}

GoalValidationResult GoalDialogValidator::validateAll(const QString& title, const QString& description,
                                                    int estimatedTime, QWidget* titleWidget,
                                                    QWidget* descriptionWidget, QWidget* timeWidget)
{
    // Validate title first (most important)
    auto titleResult = validateTitle(title, titleWidget);
    if (!titleResult.isValid) {
        return titleResult;
    }
    
    // Validate description
    auto descriptionResult = validateDescription(description, descriptionWidget);
    if (!descriptionResult.isValid) {
        return descriptionResult;
    }

    // Validate estimated time
    auto timeResult = validateEstimatedTime(estimatedTime, timeWidget);
    if (!timeResult.isValid) {
        return timeResult;
    }
    
    return GoalValidationResult(true);
}

bool GoalDialogValidator::isValidTitle(const QString& title)
{
    QString trimmedTitle = title.trimmed();
    return !trimmedTitle.isEmpty() && trimmedTitle.length() <= GoalDialogConstants::TITLE_MAX_LENGTH;
}

bool GoalDialogValidator::isValidDescription(const QString& description)
{
    return description.length() <= GoalDialogConstants::DESCRIPTION_MAX_LENGTH;
}

bool GoalDialogValidator::isValidEstimatedTime(int minutes)
{
    return minutes >= 0 && minutes <= GoalDialogConstants::TIME_INPUT_MAX_VALUE;
}