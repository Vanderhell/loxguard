#include "loxguard_ports.h"

#define LOX_PORT_FLAG_RTOS (1u << 0)
#define LOX_PORT_FLAG_MPU  (1u << 1)

static const lox_port_info_t g_ports[] = {
    { LOX_PORT_HOST, "host", 0u },
    { LOX_PORT_FREERTOS_STUB, "freertos_stub", LOX_PORT_FLAG_RTOS },
    { LOX_PORT_CORTEXM_STUB, "cortexm_stub", LOX_PORT_FLAG_MPU }
};

static lox_port_id_t g_active = LOX_PORT_HOST;

static const lox_port_info_t *find_port(lox_port_id_t id) {
    size_t i;
    for (i = 0u; i < (sizeof(g_ports) / sizeof(g_ports[0])); i++) {
        if (g_ports[i].id == id) {
            return &g_ports[i];
        }
    }
    return &g_ports[0];
}

int lox_port_set_active(lox_port_id_t id) {
    const lox_port_info_t *p = find_port(id);
    if (p->id != id) {
        return -1;
    }
    g_active = id;
    return 0;
}

lox_port_info_t lox_port_get_active(void) {
    return *find_port(g_active);
}

int lox_port_supports_rtos(lox_port_id_t id) {
    const lox_port_info_t *p = find_port(id);
    if (p->id != id) {
        return 0;
    }
    return (p->flags & LOX_PORT_FLAG_RTOS) ? 1 : 0;
}

int lox_port_supports_mpu(lox_port_id_t id) {
    const lox_port_info_t *p = find_port(id);
    if (p->id != id) {
        return 0;
    }
    return (p->flags & LOX_PORT_FLAG_MPU) ? 1 : 0;
}
