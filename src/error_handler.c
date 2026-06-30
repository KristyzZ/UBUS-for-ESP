#include <stdio.h>
#include "error_handler.h"

const char *error_string(ErrorCode error_code)
{
    switch (error_code) {

        case SUCCESS:
            return "Success";

        case ERROR_NULL_POINTER:
            return "Null pointer error";

        case ERROR_DAEMON:
            return "Failed to make program a Daemon";

        case ERROR_UNKNOWN:
        default:
            return "Unknown error";
    }
}

void print_error(ErrorCode error_code)
{
    fprintf(stderr, "ERROR: %s\n", error_string(error_code));
}