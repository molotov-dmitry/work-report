#ifndef VALUES_H
#define VALUES_H

enum TaskType
{
    TASK_ACTION,
    TASK_VACATION,
    TASK_ILLNESS,

    TASK_COUNT
};

enum TaskActionType
{
    ACTION_CREATE,
    ACTION_MODIFY,
    ACTION_TEST,
    ACTION_BUSINESS_TRIP,
    ACTION_DOCS,
    ACTION_ARCHIVE,

    ACTION_OTHER,

    ACTION_COUNT
};

enum TaskResult
{
    RESULT_DONE,
    RESULT_DONE_PART,
    RESULT_STUCK,
    RESULT_DELAYED,

    RESULT_COUNT
};

struct Values
{
    int value;
    const char* jsonValue;
    const char* displayValue;
};

extern const Values gValuesTaskTypes[TASK_COUNT];
extern const Values gValuesActionTypes[ACTION_COUNT];
extern const Values gValuesResults[RESULT_COUNT];

#endif // VALUES_H
