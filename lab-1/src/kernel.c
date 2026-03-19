#include "io_utils.h"
#include "sbi_calls.h"

extern char __bss[], __bss_end[], __stack_top[];

static int is_space(char c) {
  return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

static int streq(const char *left, const char *right) {
  while (*left && *right && *left == *right) {
    left++;
    right++;
  }
  return *left == '\0' && *right == '\0';
}

static int parse_non_negative_int(const char *text, int *value) {
  int parsed = 0;
  if (!text || *text == '\0') {
    return 0;
  }

  while (*text) {
    if (*text < '0' || *text > '9') {
      return 0;
    }
    parsed = parsed * 10 + (*text - '0');
    text++;
  }

  *value = parsed;
  return 1;
}

static char *next_token(char **cursor) {
  char *start = 0;
  if (!cursor || !*cursor) {
    return 0;
  }

  while (**cursor && is_space(**cursor)) {
    (*cursor)++;
  }

  if (**cursor == '\0') {
    return 0;
  }

  start = *cursor;
  while (**cursor && !is_space(**cursor)) {
    (*cursor)++;
  }

  if (**cursor) {
    **cursor = '\0';
    (*cursor)++;
  }

  return start;
}

static void print_help(void) {
  println("Available commands:");
  println("  help                 - Show this help message");
  println("  echo <text>          - Print text");
  println("  sbi-spec             - Show SBI specification version");
  println("  sbi-impl             - Show SBI implementation version");
  println("  pmu-count            - Show number of PMU counters");
  println("  pmu-info <index>     - Show PMU counter details");
  println("  hart-status <hart>   - Show hart status");
  println("  stop                 - Stop current hart");
  println("  shutdown             - Shut down machine");
}

static void run_shell(void) {
  char line[128];

  println("Interactive shell started. Type 'help' for commands.");

  for (;;) {
    char *cursor = line;
    char *cmd = 0;

    print("> ");
    int length = get_line(line, (int)sizeof(line) - 1);
    line[length] = '\0';

    cmd = next_token(&cursor);
    if (!cmd) {
      continue;
    }

    if (streq(cmd, "help")) {
      print_help();
      continue;
    }

    if (streq(cmd, "echo")) {
      while (*cursor && is_space(*cursor)) {
        cursor++;
      }
      println(cursor);
      continue;
    }

    if (streq(cmd, "sbi-spec")) {
      sbi_spec_version_t version = get_specification_version();
      if (version.status != SBI_STATUS_SUCCESS) {
        printf("error: %d\n", (int)version.status);
        continue;
      }
      printf("SBI spec: %u.%u (raw=0x%x)\n", (unsigned)version.major,
             (unsigned)version.minor, (unsigned)version.raw);
      continue;
    }

    if (streq(cmd, "sbi-impl")) {
      sbi_value_t impl = get_implementation_version();
      if (impl.status != SBI_STATUS_SUCCESS) {
        printf("error: %d\n", (int)impl.status);
        continue;
      }
      printf("SBI impl version: %u\n", (unsigned)impl.value);
      continue;
    }

    if (streq(cmd, "pmu-count")) {
      int count = get_counters_number();
      if (count < 0) {
        printf("error: %d\n", count);
        continue;
      }
      printf("PMU counters: %d\n", count);
      continue;
    }

    if (streq(cmd, "pmu-info")) {
      char *arg = next_token(&cursor);
      int index = 0;
      if (!arg || !parse_non_negative_int(arg, &index)) {
        println("usage: pmu-info <index>");
        continue;
      }

      sbi_value_t details = get_counter_details(index);
      if (details.status != SBI_STATUS_SUCCESS) {
        printf("error: %d\n", (int)details.status);
        continue;
      }
      printf("PMU[%d] details: 0x%x\n", index, (unsigned)details.value);
      continue;
    }

    if (streq(cmd, "hart-status")) {
      char *arg = next_token(&cursor);
      int hart = 0;
      if (!arg || !parse_non_negative_int(arg, &hart)) {
        println("usage: hart-status <hart>");
        continue;
      }

      sbi_hart_status_t status = get_hart_status(hart);
      if (status.status != SBI_STATUS_SUCCESS) {
        printf("error: %d\n", (int)status.status);
        continue;
      }
      printf("hart %d state: %d\n", hart, (int)status.state);
      continue;
    }

    if (streq(cmd, "stop")) {
      sbi_status_t status = stop_hart();
      printf("stop returned: %d\n", (int)status);
      continue;
    }

    if (streq(cmd, "shutdown")) {
      sbi_status_t status = shutdown();
      printf("shutdown returned: %d\n", (int)status);
      continue;
    }

    println("Unknown command. Type 'help'.");
  }
}

void kernel_main(void) { run_shell(); }

__attribute__((section(".text.boot"))) __attribute__((naked)) void boot(void) {
  __asm__ __volatile__("mv sp, %[stack_top]\n"
                       "j kernel_main\n"
                       :
                       : [stack_top] "r"(__stack_top));
}
