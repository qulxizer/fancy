#include "logger.h"

Loggy_t lgy;
void logger_init(FILE *file, FILE *error_file, LogLevel_t lvl) {
  lgy = loggy_init(file, error_file, lvl);
}
