#include "loxguard.h"
#include "test_common.h"

int test_arena_suite(void) {
    uint8_t mem[64];
    lox_arena_t arena;
    void *p1;
    void *p2;
    uintptr_t addr;
    int failed = 0;

    lox_arena_init(&arena, mem, sizeof(mem));
    p1 = lox_arena_alloc(&arena, 8u, 8u);
    failed |= expect(p1 != NULL, "arena alloc valid");
    failed |= expect(lox_arena_used(&arena) == 8u, "arena used after first alloc");

    p2 = lox_arena_alloc(&arena, 8u, 16u);
    failed |= expect(p2 != NULL, "arena aligned alloc valid");
    addr = (uintptr_t)p2;
    failed |= expect((addr % 16u) == 0u, "arena allocation alignment");

    failed |= expect(lox_arena_alloc(&arena, 64u, 4u) == NULL, "arena overflow returns NULL");
    {
        size_t expected_used = (size_t)((const uint8_t *)p2 - mem) + 8u;
        failed |= expect(lox_arena_used(&arena) == expected_used, "arena used unchanged after overflow");
    }
    p1 = lox_arena_alloc(&arena, 8u, 4u);
    failed |= expect(p1 != NULL, "arena remains usable after overflow failure");
    {
        size_t expected_used = (size_t)((const uint8_t *)p1 - mem) + 8u;
        failed |= expect(lox_arena_used(&arena) == expected_used, "arena used advances after post-failure alloc");
    }
    failed |= expect(lox_arena_alloc(&arena, (size_t)-1, 4u) == NULL, "arena huge size overflow returns NULL");
    failed |= expect(lox_arena_alloc(&arena, 1u, 3u) == NULL, "arena invalid align returns NULL");
    p1 = lox_arena_alloc(&arena, 0u, 4u);
    failed |= expect(p1 != NULL, "arena zero-size alloc valid");

    arena.used = arena.size + 1u;
    failed |= expect(lox_arena_alloc(&arena, 1u, 4u) == NULL, "arena corrupted used rejected");
    failed |= expect(lox_arena_remaining(&arena) == 0u, "arena remaining zero when used exceeds size");

    lox_arena_reset(&arena);
    failed |= expect(lox_arena_used(&arena) == 0u, "arena reset");
    failed |= expect(lox_arena_remaining(&arena) == sizeof(mem), "arena remaining after reset");

    return failed;
}
