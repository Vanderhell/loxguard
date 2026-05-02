#include <Arduino.h>
#include <string.h>

#if defined(ARDUINO_ARCH_ESP32)
#include <SPI.h>
#include <SD_MMC.h>
#include <FS.h>
#endif

#ifndef LOX_BENCH_LCD_ENABLE
#define LOX_BENCH_LCD_ENABLE 1
#endif

#if LOX_BENCH_LCD_ENABLE
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#endif

extern "C" {
#include "loxguard.h"
#include "loxguard_adapters.h"
#include "loxguard_format.h"
#include "loxguard_ports.h"
}

/* SDMMC pins aligned with loxdb_esp32_s3_sd_stress_bench */
#ifndef SDMMC_PIN_CLK
#define SDMMC_PIN_CLK 17
#endif
#ifndef SDMMC_PIN_CMD
#define SDMMC_PIN_CMD 18
#endif
#ifndef SDMMC_PIN_D0
#define SDMMC_PIN_D0 16
#endif
#ifndef SDMMC_PIN_D3
#define SDMMC_PIN_D3 47
#endif

/* ST7735 pins aligned with loxdb_esp32_s3_sd_stress_bench */
#ifndef LCD_PIN_SCLK
#define LCD_PIN_SCLK 10
#endif
#ifndef LCD_PIN_MOSI
#define LCD_PIN_MOSI 11
#endif
#ifndef LCD_PIN_CS
#define LCD_PIN_CS 12
#endif
#ifndef LCD_PIN_DC
#define LCD_PIN_DC 13
#endif
#ifndef LCD_PIN_RST
#define LCD_PIN_RST 14
#endif
#ifndef LCD_PIN_BL
#define LCD_PIN_BL -1
#endif

static const char *kLogPath = "/loxguard_bench.log";

typedef struct {
  uint32_t total;
  uint32_t pass;
  uint32_t fail;
  uint32_t persisted;
} run_stats_t;

typedef struct {
  const char *label;
  uint32_t runs;
  uint32_t pass;
  uint32_t fail;
} case_stats_t;

static run_stats_t g_stats = {0u, 0u, 0u, 0u};
static bool g_sd_ready = false;

#if LOX_BENCH_LCD_ENABLE
static Adafruit_ST7735 g_tft(LCD_PIN_CS, LCD_PIN_DC, LCD_PIN_RST);
static bool g_lcd_ready = false;
#endif

static const char *action_str(lox_action_t action) {
  switch (action) {
    case LOX_ACTION_DROP_INPUT: return "DROP_INPUT";
    case LOX_ACTION_RESET_BLOCK: return "RESET_BLOCK";
    default: return "NONE";
  }
}

static const char *result_str(lox_result_t result) {
  switch (result) {
    case LOX_RESULT_OK: return "OK";
    case LOX_RESULT_ERROR: return "ERROR";
    case LOX_RESULT_TIMEOUT: return "TIMEOUT";
    case LOX_RESULT_BOUNDS: return "BOUNDS";
    case LOX_RESULT_ARENA: return "ARENA";
    case LOX_RESULT_MEMORY_FAULT: return "MEMORY_FAULT";
    case LOX_RESULT_UNSUPPORTED: return "UNSUPPORTED";
    default: return "NONE";
  }
}

static const char *kind_str(lox_event_kind_t kind) {
  switch (kind) {
    case LOX_EVENT_BLOCK_WRITE_OUT_OF_BOUNDS: return "BLOCK_WRITE_OUT_OF_BOUNDS";
    case LOX_EVENT_BLOCK_ARENA_OVERFLOW: return "BLOCK_ARENA_OVERFLOW";
    case LOX_EVENT_BLOCK_TIMEOUT: return "BLOCK_TIMEOUT";
    case LOX_EVENT_BLOCK_MEMORY_FAULT: return "BLOCK_MEMORY_FAULT";
    case LOX_EVENT_BLOCK_UNSUPPORTED: return "BLOCK_UNSUPPORTED";
    case LOX_EVENT_BLOCK_ENTERED: return "BLOCK_ENTERED";
    case LOX_EVENT_BLOCK_OK: return "BLOCK_OK";
    case LOX_EVENT_BLOCK_COMPLETED: return "BLOCK_COMPLETED";
    case LOX_EVENT_BLOCK_ERROR: return "BLOCK_ERROR";
    case LOX_EVENT_BLOCK_PANIC: return "BLOCK_PANIC";
    case LOX_EVENT_BLOCK_FAULT: return "BLOCK_FAULT";
    default: return "NONE";
  }
}

static uint32_t bench_now_ms(void) {
  return (uint32_t)micros();
}

static const lox_event_t *find_last_incident(const lox_blackbox_t *bb) {
  size_t i;
  if (bb == NULL || bb->count == 0u) {
    return NULL;
  }
  for (i = bb->count; i > 0u; i--) {
    const lox_event_t *ev = &bb->events[i - 1u];
    if (ev->kind != LOX_EVENT_BLOCK_ENTERED &&
        ev->kind != LOX_EVENT_BLOCK_OK &&
        ev->kind != LOX_EVENT_BLOCK_COMPLETED) {
      return ev;
    }
  }
  return NULL;
}

static bool mount_sd() {
#if defined(ARDUINO_ARCH_ESP32)
  if (g_sd_ready) {
    return true;
  }
  if (!SD_MMC.setPins(SDMMC_PIN_CLK, SDMMC_PIN_CMD, SDMMC_PIN_D0, -1, -1, SDMMC_PIN_D3)) {
    Serial.println("[ERR] SD_MMC.setPins failed");
    return false;
  }
  if (!SD_MMC.begin("/sdcard", true, false)) {
    Serial.println("[ERR] SD_MMC.begin failed");
    return false;
  }
  if (SD_MMC.cardType() == CARD_NONE) {
    Serial.println("[ERR] no SD card");
    return false;
  }
  g_sd_ready = true;
  Serial.printf("[OK] SD mounted card=%lluMB\n", (unsigned long long)(SD_MMC.cardSize() / (1024ull * 1024ull)));
  return true;
#else
  return false;
#endif
}

static void append_log(const char *tag, const lox_report_t *report, const lox_blackbox_t *bb) {
#if defined(ARDUINO_ARCH_ESP32)
  if (!mount_sd()) {
    return;
  }
  File f = SD_MMC.open(kLogPath, FILE_APPEND);
  if (!f) {
    return;
  }

  f.printf("[%s] result=%s action=%s reason=%s persisted=%d duration=%lu\n",
           tag,
           result_str(report->result),
           action_str(report->action),
           report->reason ? report->reason : "NONE",
           report->event_persisted,
           (unsigned long)report->duration_ticks);

  if (bb != NULL && bb->count > 0u) {
    char csv[192];
    const lox_event_t *last = &bb->events[bb->count - 1u];
    if (lox_event_format_csv(last, csv, sizeof(csv)) > 0) {
      f.printf("[%s] kind=%s csv=%s\n", tag, kind_str(last->kind), csv);
    } else {
      f.printf("[%s] kind=%s\n", tag, kind_str(last->kind));
    }
  }
  f.flush();
  f.close();
#else
  (void)tag;
  (void)report;
  (void)bb;
#endif
}

static void sd_dump_tail(uint32_t max_bytes, uint32_t max_lines) {
#if defined(ARDUINO_ARCH_ESP32)
  if (!mount_sd()) {
    Serial.println("[ERR] sd not mounted");
    return;
  }
  File f = SD_MMC.open(kLogPath, FILE_READ);
  if (!f) {
    Serial.println("[ERR] open log failed");
    return;
  }

  size_t size = (size_t)f.size();
  size_t start = 0u;
  if (max_bytes > 0u && size > (size_t)max_bytes) {
    start = size - (size_t)max_bytes;
  }
  (void)f.seek((uint32_t)start);

  static char buf[2048];
  size_t n = f.readBytes(buf, (sizeof(buf) - 1u));
  buf[n] = '\0';
  f.close();

  Serial.println("[sd] --- log tail ---");

  /* If we started mid-file, we may be mid-line. Skip to the next newline
     to avoid printing a truncated first line. */
  if (start > 0u && n > 0u && buf[0] != '\n') {
    size_t j;
    for (j = 0u; j < n; j++) {
      if (buf[j] == '\n') {
        if (j + 1u < n) {
          memmove(buf, &buf[j + 1u], n - (j + 1u));
          n -= (j + 1u);
          buf[n] = '\0';
        } else {
          buf[0] = '\0';
          n = 0u;
        }
        break;
      }
    }
  }

  /* Print last max_lines lines from the loaded tail buffer. */
  if (max_lines == 0u) {
    Serial.print(buf);
    Serial.println();
    Serial.println("[sd] --- end ---");
    return;
  }

  uint32_t lines = 0u;
  size_t i;
  for (i = n; i > 0u; i--) {
    if (buf[i - 1u] == '\n') {
      lines++;
      if (lines > max_lines) {
        break;
      }
    }
  }
  if (i > 0u && i < n) {
    Serial.print(&buf[i]);
  } else {
    Serial.print(buf);
  }
  Serial.println();
  Serial.println("[sd] --- end ---");
#else
  (void)max_bytes;
  (void)max_lines;
#endif
}

static void lcd_init() {
#if LOX_BENCH_LCD_ENABLE
  if (LCD_PIN_BL >= 0) {
    pinMode(LCD_PIN_BL, OUTPUT);
    digitalWrite(LCD_PIN_BL, HIGH);
  }
  SPI.begin(LCD_PIN_SCLK, -1, LCD_PIN_MOSI, LCD_PIN_CS);
#ifdef INITR_MINI160x80
  g_tft.initR(INITR_MINI160x80);
#else
  g_tft.initR(INITR_BLACKTAB);
#endif
  g_tft.setRotation(1);
  g_tft.fillScreen(ST77XX_BLACK);
  g_lcd_ready = true;
#endif
}

static void lcd_progress(const char *step, const lox_report_t *report, const lox_blackbox_t *bb) {
#if LOX_BENCH_LCD_ENABLE
  if (!g_lcd_ready) {
    return;
  }
  g_tft.fillScreen(ST77XX_BLACK);
  g_tft.setTextSize(1);
  g_tft.setTextColor(ST77XX_YELLOW);
  g_tft.setCursor(2, 2);
  g_tft.println("loxguard bench");

  g_tft.setTextColor(ST77XX_WHITE);
  g_tft.setCursor(2, 16);
  g_tft.printf("step:%s", step);
  g_tft.setCursor(2, 28);
  g_tft.printf("res:%s", result_str(report->result));
  g_tft.setCursor(2, 40);
  g_tft.printf("act:%s", action_str(report->action));
  g_tft.setCursor(2, 52);
  g_tft.printf("pass:%lu fail:%lu", (unsigned long)g_stats.pass, (unsigned long)g_stats.fail);
  g_tft.setCursor(2, 64);
  g_tft.printf("persist:%lu/%lu", (unsigned long)g_stats.persisted, (unsigned long)g_stats.total);
  if (bb != NULL && bb->count > 0u) {
    g_tft.setCursor(2, 76);
    g_tft.printf("evt:%s", kind_str(bb->events[bb->count - 1u].kind));
  }
#else
  (void)step;
  (void)report;
  (void)bb;
#endif
}

static bool scenario_passes(const lox_report_t *report, const lox_event_t *incident, lox_result_t exp_result, lox_event_kind_t exp_incident) {
  if (report == NULL) {
    return false;
  }
  if (report->result != exp_result) {
    return false;
  }
  if (exp_incident == LOX_EVENT_NONE) {
    return incident == NULL;
  }
  if (incident == NULL) {
    return false;
  }
  return incident->kind == exp_incident;
}

static bool eval_report(const lox_report_t *report, const lox_blackbox_t *bb, lox_result_t exp_result, lox_event_kind_t exp_incident, const lox_event_t **out_incident) {
  const lox_event_t *incident = find_last_incident(bb);
  bool pass = scenario_passes(report, incident, exp_result, exp_incident);
  if (out_incident != NULL) {
    *out_incident = incident;
  }
  return pass;
}

static void stats_record(bool pass, bool persisted) {
  g_stats.total++;
  if (pass) {
    g_stats.pass++;
  } else {
    g_stats.fail++;
  }
  if (persisted) {
    g_stats.persisted++;
  }
}

static void print_report(const char *label, const lox_report_t *report, const lox_blackbox_t *bb, lox_result_t exp_result, lox_event_kind_t exp_incident, bool pass) {
  char line[192];
  const lox_event_t *last = (bb != NULL && bb->count > 0u) ? &bb->events[bb->count - 1u] : NULL;
  const lox_event_t *incident = NULL;
  (void)eval_report(report, bb, exp_result, exp_incident, &incident);

  Serial.print("[");
  Serial.print(label);
  Serial.print("] result=");
  Serial.print(result_str(report->result));
  Serial.print(" action=");
  Serial.print(action_str(report->action));
  Serial.print(" reason=");
  Serial.print(report->reason ? report->reason : "NONE");
  Serial.print(" verdict=");
  Serial.print(pass ? "PASS" : "FAIL");
  Serial.print(" persisted=");
  Serial.print(report->event_persisted);
  Serial.print(" duration_ticks=");
  Serial.println((unsigned long)report->duration_ticks);

  if (incident != NULL) {
    Serial.print("[");
    Serial.print(label);
    Serial.print("] incident_event=");
    Serial.print(kind_str(incident->kind));
    Serial.print(" block=");
    Serial.print(incident->block_name ? incident->block_name : "none");
    Serial.print(" reason=");
    Serial.println(incident->reason ? incident->reason : "none");

    if (lox_event_format_csv(incident, line, sizeof(line)) > 0) {
      Serial.print("[");
      Serial.print(label);
      Serial.print("] incident_csv=");
      Serial.println(line);
    }
  }

  if (last != NULL) {
    Serial.print("[");
    Serial.print(label);
    Serial.print("] completed_event=");
    Serial.println(kind_str(last->kind));
  }

  append_log(label, report, bb);
  lcd_progress(label, report, bb);
}

static bool run_case_once(const char *label, const lox_report_t *report, const lox_blackbox_t *bb, lox_result_t exp_result, lox_event_kind_t exp_incident, bool verbose) {
  bool pass = eval_report(report, bb, exp_result, exp_incident, NULL);
  stats_record(pass, report->event_persisted);

  if (verbose) {
    print_report(label, report, bb, exp_result, exp_incident, pass);
  }
  return pass;
}

static void print_case_summary(const case_stats_t *cs) {
  if (cs == NULL) {
    return;
  }
  Serial.print("[case] ");
  Serial.print(cs->label);
  Serial.print(" pass=");
  Serial.print((unsigned long)cs->pass);
  Serial.print("/");
  Serial.print((unsigned long)cs->runs);
  Serial.print(" fail=");
  Serial.println((unsigned long)cs->fail);
}

static void run_all_variants(void) {
  static const uint32_t kBenchItersChecked = 500u;
  static const uint32_t kBenchItersFailures = 250u;
  static const uint32_t kBenchItersBackend = 250u;
  static const uint32_t kCheckpointEvery = 50u;
  uint8_t input[8] = {0,1,2,3,4,5,6,7};
  uint8_t output_ok[32];
  uint8_t output_fail[16];
  uint8_t scratch_ok[64];
  uint8_t scratch_small[4];
  lox_blackbox_t bb;
  lox_report_t report;
  case_stats_t cases[10] = {
    {"checked_ok", 0u, 0u, 0u},
    {"checked_oob", 0u, 0u, 0u},
    {"arena_overflow", 0u, 0u, 0u},
    {"timeout_demo", 0u, 0u, 0u},
    {"panic_demo", 0u, 0u, 0u},
    {"fault_demo", 0u, 0u, 0u},
    {"rtos_host", 0u, 0u, 0u},
    {"rtos_stub", 0u, 0u, 0u},
    {"mpu_host", 0u, 0u, 0u},
    {"mpu_stub", 0u, 0u, 0u}
  };
  uint32_t i;

  memset(output_ok, 0, sizeof(output_ok));
  memset(output_fail, 0x55, sizeof(output_fail));

  g_stats.total = 0u;
  g_stats.pass = 0u;
  g_stats.fail = 0u;
  g_stats.persisted = 0u;

  Serial.println("[phase] smoke");
  report = lox_run_checked_parser_demo(input, sizeof(input), output_ok, sizeof(output_ok), scratch_ok, sizeof(scratch_ok), &bb);
  (void)run_case_once("checked_ok", &report, &bb, LOX_RESULT_OK, LOX_EVENT_NONE, true);

  report = lox_run_checked_parser_demo(input, sizeof(input), output_fail, sizeof(output_fail), scratch_ok, sizeof(scratch_ok), &bb);
  (void)run_case_once("checked_oob", &report, &bb, LOX_RESULT_BOUNDS, LOX_EVENT_BLOCK_WRITE_OUT_OF_BOUNDS, true);

  report = lox_run_checked_parser_demo(input, sizeof(input), output_fail, sizeof(output_fail), scratch_small, sizeof(scratch_small), &bb);
  (void)run_case_once("arena_overflow", &report, &bb, LOX_RESULT_ARENA, LOX_EVENT_BLOCK_ARENA_OVERFLOW, true);

  report = lox_run_checked_parser_timeout_demo(&bb);
  (void)run_case_once("timeout_demo", &report, &bb, LOX_RESULT_TIMEOUT, LOX_EVENT_BLOCK_TIMEOUT, true);

  report = lox_run_guard_panic_demo(&bb);
  (void)run_case_once("panic_demo", &report, &bb, LOX_RESULT_ERROR, LOX_EVENT_BLOCK_PANIC, true);

  report = lox_run_guard_fault_demo(&bb);
  (void)run_case_once("fault_demo", &report, &bb, LOX_RESULT_ERROR, LOX_EVENT_BLOCK_FAULT, true);

  (void)lox_port_set_active(LOX_PORT_HOST);
  report = lox_run_rtos_timeout_demo(&bb, "rtos_task_demo", 50u);
  (void)run_case_once("rtos_host", &report, &bb, LOX_RESULT_UNSUPPORTED, LOX_EVENT_BLOCK_UNSUPPORTED, true);

  (void)lox_port_set_active(LOX_PORT_FREERTOS_STUB);
  report = lox_run_rtos_timeout_demo(&bb, "rtos_task_demo", 50u);
  (void)run_case_once("rtos_stub", &report, &bb, LOX_RESULT_TIMEOUT, LOX_EVENT_BLOCK_TIMEOUT, true);

  (void)lox_port_set_active(LOX_PORT_HOST);
  report = lox_run_mpu_fault_demo(&bb, "mpu_demo", 0x20000020u, 0x12u, 0x0u);
  (void)run_case_once("mpu_host", &report, &bb, LOX_RESULT_UNSUPPORTED, LOX_EVENT_BLOCK_UNSUPPORTED, true);

  (void)lox_port_set_active(LOX_PORT_CORTEXM_STUB);
  report = lox_run_mpu_fault_demo(&bb, "mpu_demo", 0x20000020u, 0x12u, 0x0u);
  (void)run_case_once("mpu_stub", &report, &bb, LOX_RESULT_MEMORY_FAULT, LOX_EVENT_BLOCK_MEMORY_FAULT, true);

  (void)lox_port_set_active(LOX_PORT_HOST);

  Serial.println("[phase] stress_checked");
  for (i = 0u; i < kBenchItersChecked; i++) {
    report = lox_run_checked_parser_demo(input, sizeof(input), output_ok, sizeof(output_ok), scratch_ok, sizeof(scratch_ok), &bb);
    cases[0].runs++;
    if (run_case_once("checked_ok", &report, &bb, LOX_RESULT_OK, LOX_EVENT_NONE, false)) { cases[0].pass++; } else { cases[0].fail++; }

    report = lox_run_checked_parser_demo(input, sizeof(input), output_fail, sizeof(output_fail), scratch_ok, sizeof(scratch_ok), &bb);
    cases[1].runs++;
    if (run_case_once("checked_oob", &report, &bb, LOX_RESULT_BOUNDS, LOX_EVENT_BLOCK_WRITE_OUT_OF_BOUNDS, false)) { cases[1].pass++; } else { cases[1].fail++; }

    report = lox_run_checked_parser_demo(input, sizeof(input), output_fail, sizeof(output_fail), scratch_small, sizeof(scratch_small), &bb);
    cases[2].runs++;
    if (run_case_once("arena_overflow", &report, &bb, LOX_RESULT_ARENA, LOX_EVENT_BLOCK_ARENA_OVERFLOW, false)) { cases[2].pass++; } else { cases[2].fail++; }

    if (((i + 1u) % kCheckpointEvery) == 0u) {
      Serial.print("[checkpoint] stress_checked ");
      Serial.println((unsigned long)(i + 1u));
    }
  }

  Serial.println("[phase] stress_failures");
  for (i = 0u; i < kBenchItersFailures; i++) {
    report = lox_run_checked_parser_timeout_demo(&bb);
    cases[3].runs++;
    if (run_case_once("timeout_demo", &report, &bb, LOX_RESULT_TIMEOUT, LOX_EVENT_BLOCK_TIMEOUT, false)) { cases[3].pass++; } else { cases[3].fail++; }

    report = lox_run_guard_panic_demo(&bb);
    cases[4].runs++;
    if (run_case_once("panic_demo", &report, &bb, LOX_RESULT_ERROR, LOX_EVENT_BLOCK_PANIC, false)) { cases[4].pass++; } else { cases[4].fail++; }

    report = lox_run_guard_fault_demo(&bb);
    cases[5].runs++;
    if (run_case_once("fault_demo", &report, &bb, LOX_RESULT_ERROR, LOX_EVENT_BLOCK_FAULT, false)) { cases[5].pass++; } else { cases[5].fail++; }

    if (((i + 1u) % kCheckpointEvery) == 0u) {
      Serial.print("[checkpoint] stress_failures ");
      Serial.println((unsigned long)(i + 1u));
    }
  }

  Serial.println("[phase] stress_backends");
  for (i = 0u; i < kBenchItersBackend; i++) {
    (void)lox_port_set_active(LOX_PORT_HOST);
    report = lox_run_rtos_timeout_demo(&bb, "rtos_task_demo", 50u);
    cases[6].runs++;
    if (run_case_once("rtos_host", &report, &bb, LOX_RESULT_UNSUPPORTED, LOX_EVENT_BLOCK_UNSUPPORTED, false)) { cases[6].pass++; } else { cases[6].fail++; }

    (void)lox_port_set_active(LOX_PORT_FREERTOS_STUB);
    report = lox_run_rtos_timeout_demo(&bb, "rtos_task_demo", 50u);
    cases[7].runs++;
    if (run_case_once("rtos_stub", &report, &bb, LOX_RESULT_TIMEOUT, LOX_EVENT_BLOCK_TIMEOUT, false)) { cases[7].pass++; } else { cases[7].fail++; }

    (void)lox_port_set_active(LOX_PORT_HOST);
    report = lox_run_mpu_fault_demo(&bb, "mpu_demo", 0x20000020u, 0x12u, 0x0u);
    cases[8].runs++;
    if (run_case_once("mpu_host", &report, &bb, LOX_RESULT_UNSUPPORTED, LOX_EVENT_BLOCK_UNSUPPORTED, false)) { cases[8].pass++; } else { cases[8].fail++; }

    (void)lox_port_set_active(LOX_PORT_CORTEXM_STUB);
    report = lox_run_mpu_fault_demo(&bb, "mpu_demo", 0x20000020u, 0x12u, 0x0u);
    cases[9].runs++;
    if (run_case_once("mpu_stub", &report, &bb, LOX_RESULT_MEMORY_FAULT, LOX_EVENT_BLOCK_MEMORY_FAULT, false)) { cases[9].pass++; } else { cases[9].fail++; }

    if (((i + 1u) % kCheckpointEvery) == 0u) {
      Serial.print("[checkpoint] stress_backends ");
      Serial.println((unsigned long)(i + 1u));
    }
  }
  (void)lox_port_set_active(LOX_PORT_HOST);

  Serial.println("[summary] per-case");
  for (i = 0u; i < (uint32_t)(sizeof(cases) / sizeof(cases[0])); i++) {
    print_case_summary(&cases[i]);
  }
  Serial.print("[summary] pass=");
  Serial.print((unsigned long)g_stats.pass);
  Serial.print("/");
  Serial.print((unsigned long)g_stats.total);
  Serial.print(" fail=");
  Serial.println((unsigned long)g_stats.fail);
}

static void run_soak(uint32_t loops) {
  uint8_t input[8] = {0,1,2,3,4,5,6,7};
  uint8_t output_ok[32];
  uint8_t output_fail[16];
  uint8_t scratch_ok[64];
  uint8_t scratch_small[4];
  lox_blackbox_t bb;
  lox_report_t report;
  uint32_t i;
  uint32_t checksum = 0u;
  const uint32_t checkpoint = 1000u;

  memset(output_ok, 0, sizeof(output_ok));
  memset(output_fail, 0x55, sizeof(output_fail));
  g_stats.total = 0u;
  g_stats.pass = 0u;
  g_stats.fail = 0u;
  g_stats.persisted = 0u;

  Serial.print("[soak] start loops=");
  Serial.println((unsigned long)loops);

  for (i = 0u; i < loops; i++) {
    report = lox_run_checked_parser_demo(input, sizeof(input), output_ok, sizeof(output_ok), scratch_ok, sizeof(scratch_ok), &bb);
    checksum ^= (uint32_t)report.result;
    checksum ^= (uint32_t)report.action << 8;
    checksum ^= (uint32_t)report.duration_ticks;
    (void)run_case_once("soak_checked_ok", &report, &bb, LOX_RESULT_OK, LOX_EVENT_NONE, false);

    report = lox_run_checked_parser_demo(input, sizeof(input), output_fail, sizeof(output_fail), scratch_ok, sizeof(scratch_ok), &bb);
    checksum ^= (uint32_t)report.result;
    checksum ^= (uint32_t)report.action << 10;
    checksum ^= (uint32_t)report.duration_ticks;
    (void)run_case_once("soak_checked_oob", &report, &bb, LOX_RESULT_BOUNDS, LOX_EVENT_BLOCK_WRITE_OUT_OF_BOUNDS, false);

    report = lox_run_checked_parser_demo(input, sizeof(input), output_fail, sizeof(output_fail), scratch_small, sizeof(scratch_small), &bb);
    checksum ^= (uint32_t)report.result;
    checksum ^= (uint32_t)report.action << 12;
    checksum ^= (uint32_t)report.duration_ticks;
    (void)run_case_once("soak_arena", &report, &bb, LOX_RESULT_ARENA, LOX_EVENT_BLOCK_ARENA_OVERFLOW, false);

    report = lox_run_checked_parser_timeout_demo(&bb);
    checksum ^= (uint32_t)report.result;
    checksum ^= (uint32_t)report.action << 14;
    checksum ^= (uint32_t)report.duration_ticks;
    (void)run_case_once("soak_timeout", &report, &bb, LOX_RESULT_TIMEOUT, LOX_EVENT_BLOCK_TIMEOUT, false);

    if (((i + 1u) % checkpoint) == 0u) {
      Serial.print("[soak] checkpoint loops=");
      Serial.print((unsigned long)(i + 1u));
      Serial.print(" pass=");
      Serial.print((unsigned long)g_stats.pass);
      Serial.print("/");
      Serial.print((unsigned long)g_stats.total);
      Serial.print(" checksum=0x");
      Serial.println((unsigned long)checksum, HEX);
#if defined(ARDUINO_ARCH_ESP32)
      if (g_sd_ready) {
        File f = SD_MMC.open(kLogPath, FILE_APPEND);
        if (f) {
          f.printf("[soak_checkpoint] loops=%lu pass=%lu total=%lu checksum=0x%08lX\n",
                   (unsigned long)(i + 1u),
                   (unsigned long)g_stats.pass,
                   (unsigned long)g_stats.total,
                   (unsigned long)checksum);
          f.flush();
          f.close();
        }
      }
#endif
    }
  }

  Serial.print("[soak] done loops=");
  Serial.print((unsigned long)loops);
  Serial.print(" pass=");
  Serial.print((unsigned long)g_stats.pass);
  Serial.print("/");
  Serial.print((unsigned long)g_stats.total);
  Serial.print(" fail=");
  Serial.print((unsigned long)g_stats.fail);
  Serial.print(" checksum=0x");
  Serial.println((unsigned long)checksum, HEX);
}

void setup() {
  Serial.begin(115200);
  while (!Serial) { }
  lox_adapter_set_time_now(bench_now_ms);

  lcd_init();
  (void)mount_sd();
#if defined(ARDUINO_ARCH_ESP32)
  if (g_sd_ready) {
    File f = SD_MMC.open(kLogPath, FILE_APPEND);
    if (f) {
      f.println();
      f.println("=== loxguard bench boot ===");
      f.flush();
      f.close();
    }
  }
#endif

  Serial.println();
  Serial.println("loxguard bench start");
  run_all_variants();
  Serial.println("loxguard bench done");
  Serial.println("Type 'r' to re-run bench, 's' to run soak(50000), 'd' to dump SD log tail.");
}

void loop() {
  if (Serial.available() > 0) {
    int ch = Serial.read();
    if (ch == 'r' || ch == 'R') {
      Serial.println();
      Serial.println("loxguard bench rerun");
      run_all_variants();
      Serial.println("loxguard bench rerun done");
    } else if (ch == 's' || ch == 'S') {
      Serial.println();
      Serial.println("loxguard soak run");
      run_soak(50000u);
      Serial.println("loxguard soak done");
    } else if (ch == 'd' || ch == 'D') {
      Serial.println();
      sd_dump_tail(2048u, 60u);
    }
  }
}
