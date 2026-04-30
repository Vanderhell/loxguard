#ifndef LOXGUARD_TEST_COMMON_H
#define LOXGUARD_TEST_COMMON_H

int expect(int cond, const char *msg);

int test_span_suite(void);
int test_arena_suite(void);
int test_pipeline_suite(void);
int test_rtos_bridge_suite(void);
int test_guarded_module_suite(void);
int test_ports_suite(void);
int test_shell_suite(void);

#endif
