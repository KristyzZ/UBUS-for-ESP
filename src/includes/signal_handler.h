#ifndef SIGNAL_HANDLER_H
#define SIGNAL_HANDLER_H

#include <signal.h>

extern volatile sig_atomic_t signal_received;

void setup_signal_handlers(void);
void signal_info(void);

#endif