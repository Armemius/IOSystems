#include "sbi_calls.h"

#define SBI_BASE_GET_SPEC_VERSION 0
#define SBI_BASE_GET_IMPL_VERSION 2

#define SBI_HSM_HART_STOP 1
#define SBI_HSM_HART_GET_STATUS 2

#define SBI_SRST_SYSTEM_RESET 0
#define SBI_SRST_RESET_TYPE_SHUTDOWN 0
#define SBI_SRST_RESET_REASON_NONE 0

static sbi_status_t to_status(long error) { return (sbi_status_t)error; }

sbi_spec_version_t get_specification_version(void) {
  struct sbiret result =
      sbi_call(0, 0, 0, 0, 0, 0, SBI_BASE_GET_SPEC_VERSION, SBI_EXT_BASE);

  sbi_spec_version_t version = {
      .status = to_status(result.error), .raw = 0, .major = 0, .minor = 0};

  if (version.status == SBI_STATUS_SUCCESS) {
    version.raw = (unsigned long)result.value;
    version.major = (version.raw >> 24) & 0x7FUL;
    version.minor = version.raw & 0xFFFFFFUL;
  }

  return version;
}

sbi_value_t get_implementation_version(void) {
  struct sbiret result =
      sbi_call(0, 0, 0, 0, 0, 0, SBI_BASE_GET_IMPL_VERSION, SBI_EXT_BASE);

  sbi_value_t version = {.status = to_status(result.error), .value = 0};
  if (version.status == SBI_STATUS_SUCCESS) {
    version.value = (unsigned long)result.value;
  }

  return version;
}

int get_counters_number(void) {
  struct sbiret result =
      sbi_call(0, 0, 0, 0, 0, 0, SBI_EXT_CTR_NUM, SBI_EXT_PMU);
  if (result.error != SBI_STATUS_SUCCESS) {
    return (int)result.error;
  }

  return (int)result.value;
}

sbi_value_t get_counter_details(int index) {
  struct sbiret result =
      sbi_call((long)index, 0, 0, 0, 0, 0, SBI_EXT_CTR_DTLS, SBI_EXT_PMU);

  sbi_value_t details = {.status = to_status(result.error), .value = 0};
  if (details.status == SBI_STATUS_SUCCESS) {
    details.value = (unsigned long)result.value;
  }

  return details;
}

sbi_hart_status_t get_hart_status(int index) {
  struct sbiret result = sbi_call((long)index, 0, 0, 0, 0, 0,
                                  SBI_HSM_HART_GET_STATUS, SBI_EXT_HSM);

  sbi_hart_status_t hart_status = {.status = to_status(result.error),
                                   .state = SBI_HART_STATE_UNKNOWN};

  if (hart_status.status == SBI_STATUS_SUCCESS) {
    long state = result.value;
    if (state >= SBI_HART_STATE_STARTED &&
        state <= SBI_HART_STATE_RESUME_PENDING) {
      hart_status.state = (sbi_hart_state_t)state;
    }
  }

  return hart_status;
}

sbi_status_t stop_hart(void) {
  struct sbiret result =
      sbi_call(0, 0, 0, 0, 0, 0, SBI_HSM_HART_STOP, SBI_EXT_HSM);
  return to_status(result.error);
}

sbi_status_t shutdown(void) {
  struct sbiret result =
      sbi_call(SBI_SRST_RESET_TYPE_SHUTDOWN, SBI_SRST_RESET_REASON_NONE, 0, 0,
               0, 0, SBI_SRST_SYSTEM_RESET, SBI_EXT_SRST);
  return to_status(result.error);
}
