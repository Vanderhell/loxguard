#ifndef LOXGUARD_ADAPTERS_H
#define LOXGUARD_ADAPTERS_H

#include "loxguard.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef uint32_t (*lox_time_now_fn_t)(void);

int lox_adapter_log_event(const lox_event_t *event);
/* Returns an explicit status; non-OK means the event was not durably persisted. */
int lox_adapter_persist_event(const lox_event_t *event);
uint32_t lox_adapter_now_ms(void);
void lox_adapter_set_time_now(lox_time_now_fn_t fn);
void lox_adapter_watchdog_kick(void);
void lox_adapter_watchdog_observe_event(const lox_event_t *event);
int lox_adapter_watchdog_state_get(void);
int lox_adapter_watchdog_state_get_for_block(const char *block_name);
void lox_adapter_watchdog_reset(void);
int lox_adapter_recovery_allow_attempt(void);
int lox_adapter_recovery_allow_attempt_for_block(const char *block_name);
void lox_adapter_recovery_report_result(int success);
void lox_adapter_recovery_report_result_for_block(const char *block_name, int success);
int lox_adapter_recovery_state_get(void);
int lox_adapter_recovery_state_get_for_block(const char *block_name);
void lox_adapter_recovery_reset(void);
void lox_adapter_panic_hook(const char *message);
void lox_adapter_health_set(int degraded);
void lox_adapter_health_set_for_block(const char *block_name, int degraded);
int lox_adapter_health_get(void);
int lox_adapter_health_get_for_block(const char *block_name);
void lox_adapter_bus_reset_stats(void);
uint32_t lox_adapter_bus_publish_count(void);
uint32_t lox_adapter_bus_deliver_count(void);
uint8_t lox_adapter_bus_last_topic(void);
uint8_t lox_adapter_bus_last_kind(void);

int lox_adapter_nvlog_init_ram(uint32_t size_bytes);
int lox_adapter_nvlog_init_file(const char *path, uint32_t size_bytes);
void lox_adapter_nvlog_shutdown(void);
void lox_adapter_nvlog_inject_fail_after(int32_t n);
int lox_adapter_loxdb_persist_event(const lox_event_t *event);
#ifdef LOXGUARD_TESTING
typedef struct {
    int in_use;
    char block_name[64];
    uint32_t name_hash;
    int health_code;
    int watchdog_state;
    int recovery_state;
} lox_adapter_debug_block_state_t;

void lox_adapter_loxdb_reset(void);
void lox_adapter_loxdb_inject_fail(int enabled);
uint32_t lox_adapter_loxdb_persist_count(void);
void lox_adapter_debug_reset_block_states(void);
size_t lox_adapter_debug_block_state_count(void);
int lox_adapter_debug_block_state_snapshot(size_t index, lox_adapter_debug_block_state_t *out);
#endif

#ifdef __cplusplus
}
#endif

#endif
