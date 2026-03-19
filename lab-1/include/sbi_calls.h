#pragma once

#include "sbi.h"

typedef enum {
  SBI_STATUS_SUCCESS = 0,
  SBI_STATUS_FAILED = -1,
  SBI_STATUS_NOT_SUPPORTED = -2,
  SBI_STATUS_INVALID_PARAM = -3,
  SBI_STATUS_DENIED = -4,
  SBI_STATUS_INVALID_ADDRESS = -5,
  SBI_STATUS_ALREADY_AVAILABLE = -6,
  SBI_STATUS_ALREADY_STARTED = -7,
  SBI_STATUS_ALREADY_STOPPED = -8,
} sbi_status_t;

typedef struct {
  sbi_status_t status;
  unsigned long raw;
  unsigned long major;
  unsigned long minor;
} sbi_spec_version_t;

typedef struct {
  sbi_status_t status;
  unsigned long value;
} sbi_value_t;

typedef enum {
  SBI_HART_STATE_STARTED = 0,
  SBI_HART_STATE_STOPPED = 1,
  SBI_HART_STATE_START_PENDING = 2,
  SBI_HART_STATE_STOP_PENDING = 3,
  SBI_HART_STATE_SUSPENDED = 4,
  SBI_HART_STATE_SUSPEND_PENDING = 5,
  SBI_HART_STATE_RESUME_PENDING = 6,
  SBI_HART_STATE_UNKNOWN = -1,
} sbi_hart_state_t;

typedef struct {
  sbi_status_t status;
  sbi_hart_state_t state;
} sbi_hart_status_t;

// Gets SBI specification version (status + decoded major/minor).
sbi_spec_version_t get_specification_version(void);

// Get SBI implementation version (status + raw integer version).
sbi_value_t get_implementation_version(void);

// Get number of PMU counters. Returns non-negative counter count or error code.
int get_counters_number(void);

// Get details about specific PMU counter (raw info in value).
sbi_value_t get_counter_details(int index);

// Get hart status details (status + parsed state).
sbi_hart_status_t get_hart_status(int index);

// Stop the current hart.
sbi_status_t stop_hart(void);

// Shut down the system using SRST extension.
sbi_status_t shutdown(void);
