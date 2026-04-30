#include "loxguard_shell.h"

#include "loxguard_adapters.h"

#include <stdio.h>
#include <string.h>

#if defined(LOXGUARD_HAVE_MICROSH) && defined(LOXGUARD_USE_MICROSH)
#include "msh.h"

typedef struct {
    char *out;
    size_t cap;
    size_t len;
    const lox_blackbox_t *bb;
    const lox_report_t *report;
} lox_shell_ctx_t;

static void lox_shell_append(lox_shell_ctx_t *ctx, const char *s) {
    size_t n;
    if (ctx == NULL || s == NULL || ctx->out == NULL || ctx->cap == 0u) {
        return;
    }
    n = strlen(s);
    if (ctx->len + n >= ctx->cap) {
        n = (ctx->cap - 1u > ctx->len) ? (ctx->cap - 1u - ctx->len) : 0u;
    }
    if (n > 0u) {
        memcpy(ctx->out + ctx->len, s, n);
        ctx->len += n;
        ctx->out[ctx->len] = '\0';
    }
}

static void lox_shell_printer(const char *str, void *user) {
    lox_shell_append((lox_shell_ctx_t *)user, str);
}

static int lox_cmd_status(int argc, const char **argv, void *user) {
    lox_shell_ctx_t *ctx = (lox_shell_ctx_t *)user;
    (void)argc;
    (void)argv;
    if (ctx == NULL || ctx->report == NULL) {
        return -1;
    }
    (void)snprintf(
        ctx->out, ctx->cap,
        "status result=%d action=%d reason=%s persisted=%d",
        (int)ctx->report->result,
        (int)ctx->report->action,
        (ctx->report->reason == NULL) ? "NONE" : ctx->report->reason,
        ctx->report->event_persisted
    );
    ctx->len = strlen(ctx->out);
    return 0;
}

static int lox_cmd_blackbox(int argc, const char **argv, void *user) {
    const lox_shell_ctx_t *ctx = (const lox_shell_ctx_t *)user;
    (void)argc;
    (void)argv;
    if (ctx == NULL || ctx->bb == NULL || ctx->out == NULL || ctx->cap == 0u) {
        return -1;
    }
    if (ctx->bb->count == 0u) {
        (void)snprintf(ctx->out, ctx->cap, "blackbox count=0");
    } else {
        const lox_event_t *ev = &ctx->bb->events[ctx->bb->count - 1u];
        (void)snprintf(
            ctx->out, ctx->cap,
            "blackbox count=%zu last_kind=%d last_reason=%s",
            ctx->bb->count,
            (int)ev->kind,
            (ev->reason == NULL) ? "NONE" : ev->reason
        );
    }
    return 0;
}

static int lox_cmd_events(int argc, const char **argv, void *user) {
    lox_shell_ctx_t *ctx = (lox_shell_ctx_t *)user;
    (void)argc;
    (void)argv;
    if (ctx == NULL || ctx->out == NULL || ctx->cap == 0u) {
        return -1;
    }
    (void)snprintf(
        ctx->out, ctx->cap,
        "events bus_publish=%lu bus_deliver=%lu last_topic=%u last_kind=%u",
        (unsigned long)lox_adapter_bus_publish_count(),
        (unsigned long)lox_adapter_bus_deliver_count(),
        (unsigned int)lox_adapter_bus_last_topic(),
        (unsigned int)lox_adapter_bus_last_kind()
    );
    return 0;
}
#endif

int lox_shell_exec_command(
    const char *cmd,
    const lox_blackbox_t *bb,
    const lox_report_t *report,
    char *out,
    size_t out_len
) {
    if (cmd == NULL || out == NULL || out_len == 0u) {
        return LOXGUARD_ERR_NULL;
    }
    out[0] = '\0';

#if defined(LOXGUARD_HAVE_MICROSH) && defined(LOXGUARD_USE_MICROSH)
    {
        msh_t shell;
        lox_shell_ctx_t ctx;
        msh_err_t rc;
        ctx.out = out;
        ctx.cap = out_len;
        ctx.len = 0u;
        ctx.bb = bb;
        ctx.report = report;

        rc = msh_init(&shell, lox_shell_printer, &ctx);
        if (rc != MSH_OK) {
            return LOXGUARD_ERR_UNSUPPORTED;
        }
        (void)msh_register(&shell, "status", "loxguard status", lox_cmd_status);
        (void)msh_register(&shell, "blackbox", "loxguard blackbox", lox_cmd_blackbox);
        (void)msh_register(&shell, "events", "loxguard events", lox_cmd_events);
        if (strncmp(cmd, "loxguard ", 9u) == 0) {
            cmd += 9;
        }
        if (msh_exec(&shell, cmd) < 0) {
            return LOXGUARD_ERR_UNSUPPORTED;
        }
        return LOXGUARD_OK;
    }
#else
    (void)bb;
    (void)report;
    return LOXGUARD_ERR_UNSUPPORTED;
#endif
}
