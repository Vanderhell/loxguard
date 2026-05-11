#include "loxguard.h"
#include "loxguard_shell.h"
#include "test_common.h"

#include <string.h>

int test_shell_suite(void) {
    lox_blackbox_t bb;
    lox_report_t report;
    char out[256];
    int failed = 0;

    memset(&bb, 0, sizeof(bb));
    memset(&report, 0, sizeof(report));
    report.result = LOX_RESULT_OK;
    report.action = LOX_ACTION_NONE;
    report.reason = "OK";
    report.event_persisted = 0;

    {
        int rc = lox_shell_exec_command("loxguard status", &bb, &report, out, sizeof(out));
        if (rc == LOXGUARD_OK) {
            failed |= expect(strstr(out, "status") != NULL, "shell status command output");
            failed |= expect(strstr(out, "result=") != NULL, "shell status includes result");
        } else {
            failed |= expect(rc == LOXGUARD_ERR_UNSUPPORTED, "shell status returns unsupported when disabled");
            failed |= expect(out[0] == '\0', "shell status disabled leaves output empty");
        }
    }

    {
        int rc = lox_shell_exec_command("loxguard blackbox", &bb, &report, out, sizeof(out));
        if (rc == LOXGUARD_OK) {
            failed |= expect(strstr(out, "blackbox") != NULL, "shell blackbox command output");
        } else {
            failed |= expect(rc == LOXGUARD_ERR_UNSUPPORTED, "shell blackbox returns unsupported when disabled");
            failed |= expect(out[0] == '\0', "shell blackbox disabled leaves output empty");
        }
    }

    {
        int rc = lox_shell_exec_command("loxguard events", &bb, &report, out, sizeof(out));
        if (rc == LOXGUARD_OK) {
            failed |= expect(strstr(out, "events") != NULL, "shell events command output");
        } else {
            failed |= expect(rc == LOXGUARD_ERR_UNSUPPORTED, "shell events returns unsupported when disabled");
            failed |= expect(out[0] == '\0', "shell events disabled leaves output empty");
        }
    }

    return failed;
}
