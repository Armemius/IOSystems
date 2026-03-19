#include "io.h"
#include "sbi.h"

int put_char(char ch) {
  struct sbiret result =
      sbi_call(ch, SBI_NO_ARG, SBI_NO_ARG, SBI_NO_ARG, SBI_NO_ARG, SBI_NO_ARG,
               SBI_FID_DEFAULT, SBI_ECALL_0_1_PUTCHAR);
  if (result.error) {
    return result.value;
  }
  return 0;
}

char get_char() {
  struct sbiret result;
  do {
    result =
        sbi_call(SBI_NO_ARG, SBI_NO_ARG, SBI_NO_ARG, SBI_NO_ARG, SBI_NO_ARG,
                 SBI_NO_ARG, SBI_FID_DEFAULT, SBI_ECALL_0_1_GETCHAR);
  } while (result.error == SBI_ERR_FAILED);

  return result.error;
}
