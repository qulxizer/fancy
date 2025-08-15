#ifndef LOGGER_H
#define LOGGER_H
#include "loggy.h"
extern Loggy_t lgy;

void logger_init(FILE *file, FILE *error_file, LogLevel_t lvl);
#endif
