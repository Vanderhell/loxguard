#include "loxguard_adapters.h"

#if defined(LOXGUARD_USE_LOXDB) && defined(LOXGUARD_HAVE_LOXDB)
#include "lox.h"
#include "lox_port_ram.h"

#include <stdio.h>
#include <string.h>

typedef struct {
    uint32_t kind;
    uint32_t index;
    uint32_t limit;
    uint32_t aux_code;
    char block_name[64];
    char reason[64];
} lox_loxdb_event_record_t;

static lox_t g_loxdb;
static lox_storage_t g_loxdb_storage;
static uint8_t g_loxdb_mem[64u * 1024u];
static uint32_t g_loxdb_seq = 0u;
static uint32_t g_loxdb_persist_count = 0u;
static int g_loxdb_force_fail = 0;
static int g_loxdb_ready = 0;

static void loxguard_copy_cstr(char *dst, size_t dst_len, const char *src) {
    size_t n;

    if (dst == NULL || dst_len == 0u) {
        return;
    }
    n = 0u;
    if (src != NULL) {
        n = strlen(src);
        if (n >= dst_len) {
            n = dst_len - 1u;
        }
        memcpy(dst, src, n);
    }
    dst[n] = '\0';
}

static lox_timestamp_t lox_loxdb_now(void) {
    return (lox_timestamp_t)lox_adapter_now_ms();
}

static int lox_loxdb_ensure_init(void) {
    lox_cfg_t cfg;
    if (g_loxdb_ready) {
        return LOXGUARD_OK;
    }
    memset(&cfg, 0, sizeof(cfg));
    if (lox_port_ram_init(&g_loxdb_storage, (uint32_t)sizeof(g_loxdb_mem)) != LOX_OK) {
        return LOXGUARD_ERR_UNSUPPORTED;
    }
    cfg.storage = &g_loxdb_storage;
    cfg.ram_kb = 32u;
    cfg.now = lox_loxdb_now;
    cfg.kv_pct = 60u;
    cfg.ts_pct = 20u;
    cfg.rel_pct = 20u;
    cfg.wal_compact_auto = 1u;
    cfg.wal_compact_threshold_pct = 80u;
    cfg.wal_sync_mode = LOX_WAL_SYNC_FLUSH_ONLY;
    if (lox_init(&g_loxdb, &cfg) != LOX_OK) {
        lox_port_ram_deinit(&g_loxdb_storage);
        memset(&g_loxdb_storage, 0, sizeof(g_loxdb_storage));
        return LOXGUARD_ERR_UNSUPPORTED;
    }
    g_loxdb_ready = 1;
    return LOXGUARD_OK;
}

int lox_adapter_loxdb_persist_event(const lox_event_t *event) {
    lox_loxdb_event_record_t rec;
    char key[32];
    if (event == NULL) {
        return LOXGUARD_ERR_NULL;
    }
    if (g_loxdb_force_fail) {
        return LOXGUARD_ERR_UNSUPPORTED;
    }
    if (lox_loxdb_ensure_init() != LOXGUARD_OK) {
        return LOXGUARD_ERR_UNSUPPORTED;
    }
    memset(&rec, 0, sizeof(rec));
    rec.kind = (uint32_t)event->kind;
    rec.index = (uint32_t)event->index;
    rec.limit = (uint32_t)event->limit;
    rec.aux_code = event->aux_code;
    if (event->block_name != NULL) {
        loxguard_copy_cstr(rec.block_name, sizeof(rec.block_name), event->block_name);
    }
    if (event->reason != NULL) {
        loxguard_copy_cstr(rec.reason, sizeof(rec.reason), event->reason);
    }
    (void)snprintf(key, sizeof(key), "loxguard_ev_%lu", (unsigned long)g_loxdb_seq++);
    if (lox_kv_set(&g_loxdb, key, &rec, sizeof(rec), 0u) != LOX_OK) {
        return LOXGUARD_ERR_UNSUPPORTED;
    }
    g_loxdb_persist_count++;
    return LOXGUARD_OK;
}

#ifdef LOXGUARD_TESTING
void lox_adapter_loxdb_reset(void) {
    if (g_loxdb_ready) {
        (void)lox_deinit(&g_loxdb);
    }
    if (g_loxdb_storage.ctx != NULL) {
        lox_port_ram_deinit(&g_loxdb_storage);
    }
    memset(&g_loxdb, 0, sizeof(g_loxdb));
    memset(&g_loxdb_storage, 0, sizeof(g_loxdb_storage));
    memset(g_loxdb_mem, 0, sizeof(g_loxdb_mem));
    g_loxdb_seq = 0u;
    g_loxdb_persist_count = 0u;
    g_loxdb_force_fail = 0;
    g_loxdb_ready = 0;
}

void lox_adapter_loxdb_inject_fail(int enabled) {
    g_loxdb_force_fail = enabled ? 1 : 0;
}

uint32_t lox_adapter_loxdb_persist_count(void) {
    return g_loxdb_persist_count;
}
#endif /* LOXGUARD_TESTING */

#else

int lox_adapter_loxdb_persist_event(const lox_event_t *event) {
    (void)event;
    return LOXGUARD_ERR_UNSUPPORTED;
}

#ifdef LOXGUARD_TESTING
void lox_adapter_loxdb_reset(void) {
}

void lox_adapter_loxdb_inject_fail(int enabled) {
    (void)enabled;
}

uint32_t lox_adapter_loxdb_persist_count(void) {
    return 0u;
}
#endif /* LOXGUARD_TESTING */

#endif
