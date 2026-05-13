# Export Formats (Host Diagnostics)

loxguard provides two simple, line-oriented export formats intended for host diagnostics:

- Event export (schema v1, key/value line): `lox_event_format_csv(...)`
- Report export (key/value line): `lox_report_format_kv(...)`

These formats are optimized for:

- fixed buffers,
- no heap,
- easy copy/paste into logs,
- parsing back into snapshot structs.

Stability:
- Starting at `v1.0.0`, the schema and parsing rules for these lines are stable for `1.x`. See `docs/API_STABILITY.md`.

## Important behavior: reversible escaping

To keep lines parseable without losing information, `block_name` and `reason` are exported using percent-encoding for a small reserved set:

- `,`  -> `%2C`
- `\n` -> `%0A`
- `\r` -> `%0D`
- `=`  -> `%3D`
- `%`  -> `%25`

Parsers decode these sequences back to the original bytes in the returned snapshot structs (`*_parse_*_ex`).

## Snapshot buffer limits (truncation)

Decoded `block_name` and `reason` are stored in fixed-size snapshot buffers (`64` bytes including the null terminator).

- If the decoded value exceeds the snapshot capacity, it is truncated.
- Since percent-encoding expands some characters, truncation can occur earlier on the encoded form.

## Schema (v1)

Event data line:

```text
kind=<int>,block=<text>,reason=<text>,index=<size_t>,limit=<size_t>,aux=<uint>
```

Optional header line (when exporting a buffer with `include_header=1`):

```text
kind,block,reason,index,limit,aux
```

Report data line:

```text
block=<text>,reason=<text>,result=<uint>,action=<uint>,event_kind=<uint>,duration_ticks=<uint>,event_persisted=<0|1>
```
