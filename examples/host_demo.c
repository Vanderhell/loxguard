#include "loxguard.h"
#include "loxguard_ports.h"

#include <stdio.h>
#include <string.h>

static const char *action_str(lox_action_t action) {
    switch (action) {
        case LOX_ACTION_DROP_INPUT: return "DROP_INPUT";
        case LOX_ACTION_RESET_BLOCK: return "RESET_BLOCK";
        default: return "NONE";
    }
}

int main(void) {
    uint8_t input[8] = {0,1,2,3,4,5,6,7};
    uint8_t output[16];
    uint8_t scratch[64];
    lox_blackbox_t bb;
    lox_report_t report;
    lox_report_t rtos_report;
    lox_report_t mpu_report;

    memset(output, 0, sizeof(output));

    printf("[loxguard] init\n");
    printf("[guard] enter packet_parser\n");

    report = lox_run_checked_parser_demo(
        input, sizeof(input),
        output, sizeof(output),
        scratch, sizeof(scratch),
        &bb
    );

    if (bb.count > 0u) {
        const lox_event_t *e = &bb.events[bb.count - 1u];
        if (e->kind == LOX_EVENT_BLOCK_WRITE_OUT_OF_BOUNDS) {
            printf("[check] output write out of bounds index=%zu len=%zu\n", e->index, e->limit);
            printf("[event] BLOCK_WRITE_OUT_OF_BOUNDS source=%s\n", e->block_name);
        } else if (e->kind == LOX_EVENT_BLOCK_ARENA_OVERFLOW) {
            printf("[event] BLOCK_ARENA_OVERFLOW source=%s\n", e->block_name);
        } else if (e->kind == LOX_EVENT_BLOCK_TIMEOUT) {
            printf("[event] BLOCK_TIMEOUT source=%s\n", e->block_name);
        } else if (e->kind == LOX_EVENT_BLOCK_MEMORY_FAULT) {
            printf("[event] BLOCK_MEMORY_FAULT source=%s addr=%zu cfsr=%zu hfsr=%u\n", e->block_name, e->index, e->limit, e->aux_code);
        }
    }

    printf("[policy] action=%s\n", action_str(report.action));
    printf("[blackbox] stored event\n");
    printf("[report] last_failed_block=%s reason=%s action=%s mode=NORMAL\n",
           report.last_failed_block ? report.last_failed_block : "none",
           report.reason ? report.reason : "NONE",
           action_str(report.action));

    (void)lox_port_set_active(LOX_PORT_FREERTOS_STUB);
    rtos_report = lox_run_rtos_timeout_demo(&bb, "rtos_task_demo", 50u);
    printf("[event] BLOCK_TIMEOUT source=%s\n", rtos_report.last_failed_block ? rtos_report.last_failed_block : "none");
    printf("[report] last_failed_block=%s reason=%s action=%s mode=NORMAL\n",
           rtos_report.last_failed_block ? rtos_report.last_failed_block : "none",
           rtos_report.reason ? rtos_report.reason : "NONE",
           action_str(rtos_report.action));

    (void)lox_port_set_active(LOX_PORT_CORTEXM_STUB);
    mpu_report = lox_run_mpu_fault_demo(&bb, "mpu_demo", 0x20000020u, 0x12u, 0x0u);
    if (bb.count > 0u) {
        const lox_event_t *m = &bb.events[bb.count - 1u];
        printf("[event] BLOCK_MEMORY_FAULT source=%s addr=%zu cfsr=%zu hfsr=%u\n", m->block_name, m->index, m->limit, m->aux_code);
    }
    printf("[report] last_failed_block=%s reason=%s action=%s mode=NORMAL\n",
           mpu_report.last_failed_block ? mpu_report.last_failed_block : "none",
           mpu_report.reason ? mpu_report.reason : "NONE",
           action_str(mpu_report.action));

    return 0;
}
