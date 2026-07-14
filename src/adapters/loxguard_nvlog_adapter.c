#include "loxguard_adapters.h"

#if defined(LOXGUARD_USE_NVLOG) && defined(LOXGUARD_HAVE_NVLOG)
#include "nvlog.h"
#include "nvlog_posix.h"

#include <string.h>

typedef struct {
    uint32_t kind;
    uint32_t index;
    uint32_t limit;
    uint32_t aux_code;
    char block_name[64];
    char reason[64];
} lox_nvlog_event_record_t;

static nvlog_ctx_t g_nvlog;
static nvlog_posix_ctx_t g_nvlog_posix;
static nvlog_hal_t g_nvlog_hal;
static int g_nvlog_ready = 0;

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

static int lox_nvlog_init_common(uint32_t size_bytes) {
    if (size_bytes < 256u) {
        return LOXGUARD_ERR_OVERFLOW;
    }
    if (nvlog_format(&g_nvlog, &g_nvlog_hal, size_bytes) != NVLOG_OK) {
        g_nvlog_ready = 0;
        return LOXGUARD_ERR_UNSUPPORTED;
    }
    if (nvlog_mount(&g_nvlog, &g_nvlog_hal, size_bytes) != NVLOG_OK) {
        g_nvlog_ready = 0;
        return LOXGUARD_ERR_UNSUPPORTED;
    }
    g_nvlog_ready = 1;
    return LOXGUARD_OK;
}

int lox_adapter_nvlog_init_ram(uint32_t size_bytes) {
    lox_adapter_nvlog_shutdown();
    if (nvlog_posix_open_ram(&g_nvlog_posix, &g_nvlog_hal, size_bytes) != 0) {
        return LOXGUARD_ERR_UNSUPPORTED;
    }
    return lox_nvlog_init_common(size_bytes);
}

int lox_adapter_nvlog_init_file(const char *path, uint32_t size_bytes) {
    lox_adapter_nvlog_shutdown();
    if (path == NULL) {
        return LOXGUARD_ERR_NULL;
    }
    if (nvlog_posix_open_file(&g_nvlog_posix, &g_nvlog_hal, path, size_bytes) != 0) {
        return LOXGUARD_ERR_UNSUPPORTED;
    }
    return lox_nvlog_init_common(size_bytes);
}

void lox_adapter_nvlog_shutdown(void) {
    g_nvlog_ready = 0;
    memset(&g_nvlog, 0, sizeof(g_nvlog));
    memset(&g_nvlog_hal, 0, sizeof(g_nvlog_hal));
    nvlog_posix_close(&g_nvlog_posix);
}

void lox_adapter_nvlog_inject_fail_after(int32_t n) {
    nvlog_posix_inject_fail_after(&g_nvlog_posix, n);
}

int lox_adapter_persist_event(const lox_event_t *event) {
    lox_nvlog_event_record_t rec;

    if (event == NULL) {
        return LOXGUARD_ERR_NULL;
    }
    if (!g_nvlog_ready) {
#if defined(LOXGUARD_USE_LOXDB) && defined(LOXGUARD_HAVE_LOXDB)
        extern int lox_adapter_loxdb_persist_event(const lox_event_t *event);
        return lox_adapter_loxdb_persist_event(event);
#else
        return LOXGUARD_ERR_UNSUPPORTED;
#endif
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

    if (nvlog_append(&g_nvlog, &rec, (uint16_t)sizeof(rec)) != NVLOG_OK) {
#if defined(LOXGUARD_USE_LOXDB) && defined(LOXGUARD_HAVE_LOXDB)
        extern int lox_adapter_loxdb_persist_event(const lox_event_t *event);
        return lox_adapter_loxdb_persist_event(event);
#else
        return LOXGUARD_ERR_UNSUPPORTED;
#endif
    }
    return LOXGUARD_OK;
}

#else

int lox_adapter_nvlog_init_ram(uint32_t size_bytes) {
    (void)size_bytes;
    return LOXGUARD_ERR_UNSUPPORTED;
}

int lox_adapter_nvlog_init_file(const char *path, uint32_t size_bytes) {
    (void)path;
    (void)size_bytes;
    return LOXGUARD_ERR_UNSUPPORTED;
}

void lox_adapter_nvlog_shutdown(void) {
}

void lox_adapter_nvlog_inject_fail_after(int32_t n) {
    (void)n;
}

int lox_adapter_persist_event(const lox_event_t *event) {
#if defined(LOXGUARD_USE_LOXDB) && defined(LOXGUARD_HAVE_LOXDB)
    extern int lox_adapter_loxdb_persist_event(const lox_event_t *event);
    return lox_adapter_loxdb_persist_event(event);
#else
    (void)event;
    return LOXGUARD_ERR_UNSUPPORTED;
#endif
}

#endif
