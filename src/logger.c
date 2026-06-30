#include "logger.h"

void logger_init(void)
{
    openlog("tuya_daemon", LOG_PID | LOG_CONS, LOG_DAEMON);
}

void logger_close(void)
{
    closelog();
}