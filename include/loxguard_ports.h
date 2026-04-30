#ifndef LOXGUARD_PORTS_H
#define LOXGUARD_PORTS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    LOX_PORT_HOST = 0,
    LOX_PORT_FREERTOS_STUB = 1,
    LOX_PORT_CORTEXM_STUB = 2
} lox_port_id_t;

typedef struct {
    lox_port_id_t id;
    const char *name;
    uint32_t flags;
} lox_port_info_t;

int lox_port_set_active(lox_port_id_t id);
lox_port_info_t lox_port_get_active(void);
int lox_port_supports_rtos(lox_port_id_t id);
int lox_port_supports_mpu(lox_port_id_t id);

#ifdef __cplusplus
}
#endif

#endif
