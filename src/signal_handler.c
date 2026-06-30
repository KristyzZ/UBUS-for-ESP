#include <stdio.h>
#include <signal.h>
#include <syslog.h>

#include "signal_handler.h"

volatile sig_atomic_t signal_received = 0;

static void signal_handler(int signal)
{
    signal_received = signal;
}

void setup_signal_handlers(void)
{
    signal(SIGINT,signal_handler);
    signal(SIGTERM,signal_handler);
    signal(SIGQUIT,signal_handler);
}

void signal_info()
{
    switch(signal_received) {
    case SIGINT:
        printf("\nReceived SIGINT\n");
        syslog(LOG_WARNING, "signal SIGINT received (%d)", signal_received);
        break;

    case SIGTERM:
        printf("\nReceived SIGTERM\n");
        syslog(LOG_WARNING, "signal SIGTERM received (%d)", signal_received);
        break;

    case SIGQUIT:
        printf("\nReceived SIGQUIT");
        syslog(LOG_WARNING, "signal SIGQUIT received (%d)", signal_received);
        break;

    default:
        printf("\nReceived signal: %d\n", signal_received);
        break;
    }
}