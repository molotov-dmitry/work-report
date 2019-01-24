#include "values.h"

const Values gValuesTaskTypes[TASK_COUNT] =
{
    {TASK_ACTION,   "action",   "Задача"},
    {TASK_VACATION, "vacation", "Отпуск/отгул"},
    {TASK_ILLNESS,  "illness",  "Больничный"}
};

const Values gValuesActionTypes[ACTION_COUNT] =
{
    {ACTION_CREATE,         "create",           "Разработка"},
    {ACTION_MODIFY,         "modify",           "Доработка"},
    {ACTION_TEST,           "test",             "Тестирование"},
    {ACTION_BUSINESS_TRIP,  "business_trip",    "Командировка"},
    {ACTION_DOCS,           "docs",             "Работа с документацией"},
    {ACTION_ARCHIVE,        "archive",          "Сдача в архив"},

    {ACTION_OTHER,          "other",            "Другое"},
};

const Values gValuesResults[RESULT_COUNT] =
{
    {RESULT_DONE,           "done",             "Выполнено"},
    {RESULT_DONE_PART,      "part",             "Выполнено частично"},
    {RESULT_STUCK,          "stuck",            "Возникли трудности"},
    {RESULT_DELAYED,        "delayed",          "Отложено"}
};
