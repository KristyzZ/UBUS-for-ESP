#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

typedef enum {
    SUCCESS = 0,

    ERROR_NULL_POINTER,
    ERROR_DAEMON,
    ERROR_UNKNOWN
} ErrorCode;

const char *error_string(ErrorCode error_code);

void print_error(ErrorCode error_code);

#endif