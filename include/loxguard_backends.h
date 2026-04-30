#ifndef LOXGUARD_BACKENDS_H
#define LOXGUARD_BACKENDS_H

#include "loxguard.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint32_t fault_addr;
    uint32_t cfsr;
    uint32_t hfsr;
} lox_mpu_fault_ctx_t;

int lox_rtos_report_timeout(lox_blackbox_t *blackbox, const char *block_name, uint32_t tick_budget);
int lox_mpu_report_fault(lox_blackbox_t *blackbox, const char *block_name, const lox_mpu_fault_ctx_t *fault);

#ifdef __cplusplus
}
#endif

#endif
