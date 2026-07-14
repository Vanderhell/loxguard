#include "loxguard.h"

#include <cstring>

static int consumer_block(lox_guard_ctx_t *g, void *user_ctx) {
    (void)g;
    (void)user_ctx;
    return LOXGUARD_OK;
}

int main() {
    lox_blackbox_t bb;
    loxguard_block_cfg_t cfg;

    lox_blackbox_init(&bb);
    std::memset(&cfg, 0, sizeof(cfg));
    cfg.name = "consumer_install_find_package_cpp";
    cfg.timeout_ms = 1;
    cfg.criticality = LOXGUARD_OPTIONAL;
    cfg.max_failures = 1;
    cfg.blackbox = &bb;

    return (loxguard_run(&cfg, consumer_block, NULL).result == LOX_RESULT_OK) ? 0 : 1;
}
