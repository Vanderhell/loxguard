# Memory Contract Template

## Guard Block

Name:

## Read spans

| Name | Pointer source | Length | Notes |
|---|---|---|---|

## Write spans

| Name | Pointer source | Length | Notes |
|---|---|---|---|

## Scratch arena

| Name | Size | Alignment | Notes |
|---|---|---|---|

## Limits

```text
max_stack:
max_recursion:
timeout_ms:
max_failures:
```

## Allowed calls

- TBD

## Forbidden calls

- malloc
- free
- raw memcpy unless explicitly allowed
- raw memset unless explicitly allowed

## Expected violation behavior

```text
violation -> event -> policy -> blackbox
```
