#include "loxguard.h"
#include "loxguard_format.h"
#include "loxguard_ports.h"

#include <stdio.h>

static const char *action_str(lox_action_t action) {
    switch (action) {
        case LOX_ACTION_DROP_INPUT: return "DROP_INPUT";
        case LOX_ACTION_RESET_BLOCK: return "RESET_BLOCK";
        default: return "NONE";
    }
}

static void print_csv_line(const char *line, void *user_ctx) {
    (void)user_ctx;
    printf("[csv] %s\n", line);
}

int main(void) {
    lox_blackbox_t bb;
    lox_report_t rtos_report;
    lox_report_t mpu_report;
    char line[160];
    char dump[320];
    char report_line[160];

    printf("[loxguard] backend-demo init\n");

    (void)lox_port_set_active(LOX_PORT_FREERTOS_STUB);
    rtos_report = lox_run_rtos_timeout_demo(&bb, "rtos_task_demo", 50u);
    (void)lox_blackbox_last_event_format_csv(&bb, line, sizeof(line));
    printf("[csv-last] %s\n", line);
    (void)lox_blackbox_export_csv_lines(&bb, 1u, print_csv_line, NULL);
    (void)lox_blackbox_export_csv_buffer(&bb, 1u, dump, sizeof(dump));
    printf("[csv-buffer]\n%s\n", dump);
    (void)lox_blackbox_export_csv_buffer_ex(&bb, 1u, 1, dump, sizeof(dump));
    printf("[csv-buffer-header]\n%s\n", dump);
    printf("[schema-compatible] %d\n", lox_csv_schema_is_compatible(dump));
    if (bb.count >= 2u) {
        (void)lox_report_format_kv(&rtos_report, &bb.events[bb.count - 2u], report_line, sizeof(report_line));
    } else {
        (void)lox_report_format_kv(&rtos_report, &bb.events[bb.count - 1u], report_line, sizeof(report_line));
    }
    printf("[report-kv] %s\n", report_line);
    printf("[report] block=%s reason=%s action=%s\n",
           rtos_report.last_failed_block ? rtos_report.last_failed_block : "none",
           rtos_report.reason ? rtos_report.reason : "NONE",
           action_str(rtos_report.action));

    (void)lox_port_set_active(LOX_PORT_CORTEXM_STUB);
    mpu_report = lox_run_mpu_fault_demo(&bb, "mpu_demo", 0x20000020u, 0x12u, 0x0u);
    (void)lox_blackbox_last_event_format_csv(&bb, line, sizeof(line));
    printf("[csv-last] %s\n", line);
    (void)lox_blackbox_export_csv_lines(&bb, 2u, print_csv_line, NULL);
    (void)lox_blackbox_export_csv_buffer(&bb, 2u, dump, sizeof(dump));
    printf("[csv-buffer]\n%s\n", dump);
    (void)lox_blackbox_export_csv_buffer_ex(&bb, 2u, 1, dump, sizeof(dump));
    printf("[csv-buffer-header]\n%s\n", dump);
    printf("[schema-compatible] %d\n", lox_csv_schema_is_compatible(dump));
    if (bb.count >= 2u) {
        (void)lox_report_format_kv(&mpu_report, &bb.events[bb.count - 2u], report_line, sizeof(report_line));
    } else {
        (void)lox_report_format_kv(&mpu_report, &bb.events[bb.count - 1u], report_line, sizeof(report_line));
    }
    printf("[report-kv] %s\n", report_line);
    printf("[report] block=%s reason=%s action=%s\n",
           mpu_report.last_failed_block ? mpu_report.last_failed_block : "none",
           mpu_report.reason ? mpu_report.reason : "NONE",
           action_str(mpu_report.action));

    if (rtos_report.action != LOX_ACTION_DROP_INPUT || mpu_report.action != LOX_ACTION_DROP_INPUT) {
        return 1;
    }

    printf("PASS: backend demo flow\n");
    return 0;
}
