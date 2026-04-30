#include "loxguard_format.h"

#include <stdio.h>
#include <string.h>

static const char *k_csv_schema_header = "kind,block,reason,index,limit,aux";

int lox_report_parse_kv_ex(const char *line, lox_report_snapshot_t *out_snapshot);
int lox_event_parse_csv_line_ex(const char *line, lox_event_snapshot_t *out_snapshot);

static int lox_event_kind_is_valid(int kind) {
    return kind >= (int)LOX_EVENT_NONE && kind <= (int)LOX_EVENT_BLOCK_ERROR;
}

static int lox_action_is_valid(unsigned int action) {
    return action <= (unsigned int)LOX_ACTION_RESET_BLOCK;
}

size_t lox_event_format_csv(const lox_event_t *event, char *out, size_t out_len) {
    int n;

    if (out == NULL || out_len == 0u) {
        return 0u;
    }

    if (event == NULL) {
        n = snprintf(out, out_len, "kind=0,block=none,reason=none,index=0,limit=0,aux=0");
    } else {
        n = snprintf(
            out,
            out_len,
            "kind=%d,block=%s,reason=%s,index=%zu,limit=%zu,aux=%u",
            (int)event->kind,
            event->block_name ? event->block_name : "none",
            event->reason ? event->reason : "none",
            event->index,
            event->limit,
            event->aux_code
        );
    }

    if (n < 0) {
        out[0] = '\0';
        return 0u;
    }

    if ((size_t)n >= out_len) {
        return out_len - 1u;
    }
    return (size_t)n;
}

size_t lox_blackbox_last_event_format_csv(const lox_blackbox_t *bb, char *out, size_t out_len) {
    if (bb == NULL || bb->count == 0u) {
        return lox_event_format_csv(NULL, out, out_len);
    }
    return lox_event_format_csv(&bb->events[bb->count - 1u], out, out_len);
}

size_t lox_blackbox_export_csv_lines(
    const lox_blackbox_t *bb,
    size_t max_events,
    lox_line_writer_t writer,
    void *user_ctx
) {
    size_t start;
    size_t i;
    size_t emitted;
    char line[160];

    if (bb == NULL || writer == NULL || bb->count == 0u || max_events == 0u) {
        return 0u;
    }

    start = 0u;
    if (bb->count > max_events) {
        start = bb->count - max_events;
    }

    emitted = 0u;
    for (i = start; i < bb->count; i++) {
        (void)lox_event_format_csv(&bb->events[i], line, sizeof(line));
        writer(line, user_ctx);
        emitted++;
    }
    return emitted;
}

size_t lox_blackbox_export_csv_buffer(
    const lox_blackbox_t *bb,
    size_t max_events,
    char *out,
    size_t out_len
) {
    return lox_blackbox_export_csv_buffer_ex(bb, max_events, 0, out, out_len);
}

size_t lox_blackbox_export_csv_buffer_ex(
    const lox_blackbox_t *bb,
    size_t max_events,
    int include_header,
    char *out,
    size_t out_len
) {
    size_t start;
    size_t i;
    size_t pos;
    size_t written;
    char line[160];

    if (out == NULL || out_len == 0u) {
        return 0u;
    }
    out[0] = '\0';
    pos = 0u;

    if (include_header) {
        int n = snprintf(out + pos, out_len - pos, "%s\n", k_csv_schema_header);
        if (n < 0) {
            out[0] = '\0';
            return 0u;
        }
        if ((size_t)n >= (out_len - pos)) {
            pos = out_len - 1u;
            out[pos] = '\0';
            return pos;
        }
        pos += (size_t)n;
    }

    if (bb == NULL || bb->count == 0u || max_events == 0u) {
        out[pos] = '\0';
        return pos;
    }

    start = 0u;
    if (bb->count > max_events) {
        start = bb->count - max_events;
    }

    for (i = start; i < bb->count; i++) {
        written = lox_event_format_csv(&bb->events[i], line, sizeof(line));
        if (written == 0u && line[0] == '\0') {
            continue;
        }

        if (pos < out_len - 1u) {
            size_t copy = written;
            if (copy > (out_len - 1u - pos)) {
                copy = out_len - 1u - pos;
            }
            for (size_t j = 0u; j < copy; j++) {
                out[pos + j] = line[j];
            }
            pos += copy;
        } else {
            break;
        }

        if (i + 1u < bb->count && pos < out_len - 1u) {
            out[pos++] = '\n';
        }
    }

    out[pos] = '\0';
    return pos;
}

int lox_csv_schema_is_compatible(const char *header_line) {
    size_t n;
    if (header_line == NULL) {
        return 0;
    }
    n = strlen(k_csv_schema_header);
    if (strncmp(header_line, k_csv_schema_header, n) != 0) {
        return 0;
    }
    if (header_line[n] == '\0' || header_line[n] == '\n' || header_line[n] == '\r') {
        return 1;
    }
    return 0;
}

int lox_event_parse_csv_line(const char *line, lox_event_t *out_event) {
    lox_event_snapshot_t snap;

    if (line == NULL || out_event == NULL) {
        return 0;
    }
    if (!lox_event_parse_csv_line_ex(line, &snap)) {
        return 0;
    }
    *out_event = snap.event;
    out_event->block_name = NULL;
    out_event->reason = NULL;
    return 1;
}

int lox_event_parse_csv_line_ex(const char *line, lox_event_snapshot_t *out_snapshot) {
    int kind;
    unsigned int aux;
    char block[64];
    char reason[64];
    size_t index;
    size_t limit;
    char tail;
    int n;

    if (line == NULL || out_snapshot == NULL) {
        return 0;
    }

    n = sscanf(
        line,
        "kind=%d,block=%63[^,],reason=%63[^,],index=%zu,limit=%zu,aux=%u%c",
        &kind,
        block,
        reason,
        &index,
        &limit,
        &aux,
        &tail
    );
    if (n != 6) {
        return 0;
    }
    if (!lox_event_kind_is_valid(kind)) {
        return 0;
    }

    memcpy(out_snapshot->block_name, block, sizeof(out_snapshot->block_name));
    memcpy(out_snapshot->reason, reason, sizeof(out_snapshot->reason));
    out_snapshot->block_name[sizeof(out_snapshot->block_name) - 1u] = '\0';
    out_snapshot->reason[sizeof(out_snapshot->reason) - 1u] = '\0';

    out_snapshot->event.kind = (lox_event_kind_t)kind;
    out_snapshot->event.block_name = out_snapshot->block_name;
    out_snapshot->event.reason = out_snapshot->reason;
    out_snapshot->event.index = index;
    out_snapshot->event.limit = limit;
    out_snapshot->event.aux_code = aux;
    return 1;
}

size_t lox_blackbox_import_csv_buffer(const char *csv, lox_blackbox_t *out_bb) {
    const char *p;
    size_t imported;

    if (csv == NULL || out_bb == NULL) {
        return 0u;
    }

    out_bb->count = 0u;
    imported = 0u;
    p = csv;

    while (*p != '\0') {
        const char *line_start = p;
        const char *line_end = p;
        size_t line_len;
        char line[160];
        lox_event_snapshot_t snap;

        while (*line_end != '\0' && *line_end != '\n' && *line_end != '\r') {
            line_end++;
        }

        line_len = (size_t)(line_end - line_start);
        if (line_len > 0u) {
            size_t copy = line_len;
            if (copy >= sizeof(line)) {
                copy = sizeof(line) - 1u;
            }
            memcpy(line, line_start, copy);
            line[copy] = '\0';

            if (!lox_csv_schema_is_compatible(line) && lox_event_parse_csv_line_ex(line, &snap) == 1) {
                lox_blackbox_store(out_bb, &snap.event);
                imported++;
            }
        }

        p = line_end;
        while (*p == '\n' || *p == '\r') {
            p++;
        }
    }

    return imported;
}

size_t lox_report_format_kv(const lox_report_t *report, const lox_event_t *event, char *out, size_t out_len) {
    int n;
    const char *block;
    const char *reason;
    unsigned int kind;

    if (out == NULL || out_len == 0u) {
        return 0u;
    }

    if (report == NULL) {
        n = snprintf(out, out_len, "block=none,reason=NONE,result=0,action=0,event_kind=0,duration_ticks=0,event_persisted=0");
    } else {
        block = (report->last_block == NULL) ? "none" : report->last_block;
        reason = (report->reason == NULL) ? "NONE" : report->reason;
        kind = (event == NULL) ? 0u : (unsigned int)event->kind;
        n = snprintf(
            out,
            out_len,
            "block=%s,reason=%s,result=%u,action=%u,event_kind=%u,duration_ticks=%u,event_persisted=%u",
            block,
            reason,
            (unsigned int)report->result,
            (unsigned int)report->action,
            kind,
            report->duration_ticks,
            (unsigned int)(report->event_persisted ? 1u : 0u)
        );
    }

    if (n < 0) {
        out[0] = '\0';
        return 0u;
    }
    if ((size_t)n >= out_len) {
        return out_len - 1u;
    }
    return (size_t)n;
}

int lox_report_parse_kv(const char *line, lox_report_t *out_report, lox_event_kind_t *out_event_kind) {
    lox_report_snapshot_t snap;

    if (line == NULL || out_report == NULL || out_event_kind == NULL) {
        return 0;
    }
    if (!lox_report_parse_kv_ex(line, &snap)) {
        return 0;
    }
    *out_report = snap.report;
    *out_event_kind = snap.event_kind;
    return 1;
}

int lox_report_parse_kv_ex(const char *line, lox_report_snapshot_t *out_snapshot) {
    char block[64];
    char reason[64];
    unsigned int result;
    unsigned int action;
    unsigned int kind;
    unsigned int duration_ticks;
    unsigned int persisted;
    char tail;
    int n;

    if (line == NULL || out_snapshot == NULL) {
        return 0;
    }

    n = sscanf(
        line,
        "block=%63[^,],reason=%63[^,],result=%u,action=%u,event_kind=%u,duration_ticks=%u,event_persisted=%u%c",
        block,
        reason,
        &result,
        &action,
        &kind,
        &duration_ticks,
        &persisted,
        &tail
    );
    if (n != 7) {
        return 0;
    }
    if (!lox_action_is_valid(action) || !lox_event_kind_is_valid((int)kind) || result > (unsigned int)LOX_RESULT_UNSUPPORTED || persisted > 1u) {
        return 0;
    }

    memcpy(out_snapshot->block_name, block, sizeof(out_snapshot->block_name));
    memcpy(out_snapshot->reason, reason, sizeof(out_snapshot->reason));
    out_snapshot->block_name[sizeof(out_snapshot->block_name) - 1u] = '\0';
    out_snapshot->reason[sizeof(out_snapshot->reason) - 1u] = '\0';

    out_snapshot->report.last_failed_block = out_snapshot->block_name;
    out_snapshot->report.last_block = out_snapshot->block_name;
    out_snapshot->report.reason = out_snapshot->reason;
    out_snapshot->report.result = (lox_result_t)result;
    out_snapshot->report.action = (lox_action_t)action;
    out_snapshot->report.duration_ticks = duration_ticks;
    out_snapshot->report.event_persisted = (persisted == 1u) ? 1 : 0;
    out_snapshot->event_kind = (lox_event_kind_t)kind;
    return 1;
}
