#include "loxguard_backends.h"

#include <string.h>

static void store_event(
    lox_blackbox_t *blackbox,
    lox_event_kind_t kind,
    const char *block_name,
    const char *reason,
    size_t index,
    size_t limit,
    uint32_t aux_code
) {
    lox_event_t e;
    memset(&e, 0, sizeof(e));
    e.kind = kind;
    e.block_name = block_name;
    e.reason = reason;
    e.index = index;
    e.limit = limit;
    e.aux_code = aux_code;
    lox_blackbox_store(blackbox, &e);
}

int lox_rtos_report_timeout(lox_blackbox_t *blackbox, const char *block_name, uint32_t tick_budget) {
    if (blackbox == NULL || block_name == NULL) {
        return LOXGUARD_ERR_NULL;
    }
    store_event(blackbox, LOX_EVENT_BLOCK_TIMEOUT, block_name, "RTOS_TIMEOUT", (size_t)tick_budget, 0u, 0u);
    return LOXGUARD_OK;
}

int lox_mpu_report_fault(lox_blackbox_t *blackbox, const char *block_name, const lox_mpu_fault_ctx_t *fault) {
    if (blackbox == NULL || block_name == NULL || fault == NULL) {
        return LOXGUARD_ERR_NULL;
    }
    store_event(
        blackbox,
        LOX_EVENT_BLOCK_MEMORY_FAULT,
        block_name,
        "MPU_FAULT",
        (size_t)fault->fault_addr,
        (size_t)fault->cfsr,
        fault->hfsr
    );
    return LOXGUARD_OK;
}
