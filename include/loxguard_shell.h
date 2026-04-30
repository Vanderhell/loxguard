#ifndef LOXGUARD_SHELL_H
#define LOXGUARD_SHELL_H

#include "loxguard.h"

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

int lox_shell_exec_command(
    const char *cmd,
    const lox_blackbox_t *bb,
    const lox_report_t *report,
    char *out,
    size_t out_len
);

#ifdef __cplusplus
}
#endif

#endif
