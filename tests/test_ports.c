#include "loxguard_ports.h"
#include "test_common.h"

int test_ports_suite(void) {
    lox_port_info_t info;
    int failed = 0;

    failed |= expect(lox_port_set_active(LOX_PORT_HOST) == 0, "set host port");
    info = lox_port_get_active();
    failed |= expect(info.id == LOX_PORT_HOST, "active host id");
    failed |= expect(lox_port_supports_rtos(LOX_PORT_HOST) == 0, "host no rtos");
    failed |= expect(lox_port_supports_mpu(LOX_PORT_HOST) == 0, "host no mpu");

    failed |= expect(lox_port_set_active(LOX_PORT_FREERTOS_STUB) == 0, "set freertos stub");
    info = lox_port_get_active();
    failed |= expect(info.id == LOX_PORT_FREERTOS_STUB, "active freertos id");
    failed |= expect(lox_port_supports_rtos(LOX_PORT_FREERTOS_STUB) == 1, "freertos supports rtos");
    failed |= expect(lox_port_supports_mpu(LOX_PORT_FREERTOS_STUB) == 0, "freertos no mpu");

    failed |= expect(lox_port_set_active(LOX_PORT_CORTEXM_STUB) == 0, "set cortexm stub");
    info = lox_port_get_active();
    failed |= expect(info.id == LOX_PORT_CORTEXM_STUB, "active cortexm id");
    failed |= expect(lox_port_supports_rtos(LOX_PORT_CORTEXM_STUB) == 0, "cortexm no rtos");
    failed |= expect(lox_port_supports_mpu(LOX_PORT_CORTEXM_STUB) == 1, "cortexm supports mpu");

    failed |= expect(lox_port_set_active((lox_port_id_t)99) != 0, "invalid port rejected");

    return failed;
}
