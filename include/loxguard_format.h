#ifndef LOXGUARD_FORMAT_H
#define LOXGUARD_FORMAT_H

#include <stddef.h>

#include "loxguard.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*lox_line_writer_t)(const char *line, void *user_ctx);

typedef struct {
    lox_report_t report;
    lox_event_kind_t event_kind;
    char block_name[64];
    char reason[64];
} lox_report_snapshot_t;

typedef struct {
    lox_event_t event;
    char block_name[64];
    char reason[64];
} lox_event_snapshot_t;

size_t lox_event_format_csv(const lox_event_t *event, char *out, size_t out_len);
size_t lox_blackbox_last_event_format_csv(const lox_blackbox_t *bb, char *out, size_t out_len);
size_t lox_blackbox_export_csv_lines(
    const lox_blackbox_t *bb,
    size_t max_events,
    lox_line_writer_t writer,
    void *user_ctx
);
size_t lox_blackbox_export_csv_buffer(
    const lox_blackbox_t *bb,
    size_t max_events,
    char *out,
    size_t out_len
);
size_t lox_blackbox_export_csv_buffer_ex(
    const lox_blackbox_t *bb,
    size_t max_events,
    int include_header,
    char *out,
    size_t out_len
);
int lox_csv_schema_is_compatible(const char *header_line);
int lox_event_parse_csv_line(const char *line, lox_event_t *out_event);
int lox_event_parse_csv_line_ex(const char *line, lox_event_snapshot_t *out_snapshot);
size_t lox_blackbox_import_csv_buffer(const char *csv, lox_blackbox_t *out_bb);
size_t lox_report_format_kv(const lox_report_t *report, const lox_event_t *event, char *out, size_t out_len);
int lox_report_parse_kv(const char *line, lox_report_t *out_report, lox_event_kind_t *out_event_kind);
int lox_report_parse_kv_ex(const char *line, lox_report_snapshot_t *out_snapshot);

#ifdef __cplusplus
}
#endif

#endif
