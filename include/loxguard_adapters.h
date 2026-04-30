#ifndef LOXGUARD_ADAPTERS_H
#define LOXGUARD_ADAPTERS_H

#include "loxguard.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef uint32_t (*lox_time_now_fn_t)(void);

int lox_adapter_log_event(const lox_event_t *event);
int lox_adapter_persist_event(const lox_event_t *event);
uint32_t lox_adapter_now_ms(void);
void lox_adapter_set_time_now(lox_time_now_fn_t fn);
void lox_adapter_watchdog_kick(void);
void lox_adapter_watchdog_observe_event(const lox_event_t *event);
int lox_adapter_watchdog_state_get(void);
void lox_adapter_watchdog_reset(void);
void lox_adapter_panic_hook(const char *message);
void lox_adapter_health_set(int degraded);
int lox_adapter_health_get(void);

int lox_adapter_nvlog_init_ram(uint32_t size_bytes);
int lox_adapter_nvlog_init_file(const char *path, uint32_t size_bytes);
void lox_adapter_nvlog_shutdown(void);
void lox_adapter_nvlog_inject_fail_after(int32_t n);

#ifdef __cplusplus
}
#endif

#endif
