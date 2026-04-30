#include "loxguard_rtos_bridge.h"
#include "loxguard_backends.h"

int lox_rtos_check_task_budget(lox_blackbox_t *blackbox, const lox_rtos_task_probe_t *probe) {
    if (blackbox == NULL || probe == NULL || probe->task_name == NULL) {
        return LOXGUARD_ERR_NULL;
    }

    if (probe->elapsed_ticks > probe->tick_budget) {
        return lox_rtos_report_timeout(blackbox, probe->task_name, probe->tick_budget);
    }

    return LOXGUARD_OK;
}
