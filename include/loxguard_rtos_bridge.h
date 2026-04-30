#ifndef LOXGUARD_RTOS_BRIDGE_H
#define LOXGUARD_RTOS_BRIDGE_H

#include "loxguard.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    const char *task_name;
    uint32_t tick_budget;
    uint32_t elapsed_ticks;
} lox_rtos_task_probe_t;

int lox_rtos_check_task_budget(lox_blackbox_t *blackbox, const lox_rtos_task_probe_t *probe);

#ifdef __cplusplus
}
#endif

#endif
