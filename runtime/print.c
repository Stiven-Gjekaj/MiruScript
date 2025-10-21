#include "print.h"
#include <stdio.h>

void miru_print_int(long value) {
    printf("%ld\n", value);
}

void miru_print_float(double value) {
    printf("%f\n", value);
}

void miru_print_string(const char *value) {
    if (value) {
        printf("%s\n", value);
    }
}

void miru_print_bool(int value) {
    printf("%s\n", value ? "true" : "false");
}

void miru_print_newline(void) {
    printf("\n");
}
