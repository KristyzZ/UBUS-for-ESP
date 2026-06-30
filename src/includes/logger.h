#ifndef LOGGER_H
#define LOGGER_H

#include <syslog.h>

void logger_init(void);
void logger_close(void);

#endif