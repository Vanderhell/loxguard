# Blackbox

## Purpose

The blackbox is local evidence of what failed.

It answers:

```text
What failed?
Why?
How often?
What did loxguard do?
What mode is the system in?
```

## Minimum data

```text
boot_count
last_reset_reason
last_failed_source
last_event_type
last_action_taken
system_mode
failure_count
recent_events[N]
```

## MVP report

```text
loxguard blackbox report
------------------------
last_failed_block: packet_parser
last_event: BLOCK_ERROR
last_action: DROP_INPUT
system_mode: NORMAL
events_stored: 4
```

## Backends

- RAM ring buffer first,
- nvlog later,
- microlog output,
- microcbor export,
- loxdb optional advanced history.

## Rule

MVP blackbox must work without flash and without loxdb.

Persistence is a profile/backend, not a core requirement.
