#ifndef LOXGUARD_TEST_COMMON_H
#define LOXGUARD_TEST_COMMON_H

int expect(int cond, const char *msg);
void expect_reset(void);
int expect_fail_count(void);
int expect_total_count(void);
void expect_print_summary(void);

int test_span_suite(void);
int test_arena_suite(void);
int test_pipeline_suite(void);
int test_rtos_bridge_suite(void);
int test_guarded_module_suite(void);
int test_ports_suite(void);
int test_shell_suite(void);

#endif
