#include "io_utils.h"

extern char __bss[], __bss_end[], __stack_top[];

void kernel_main(void) {
  printf("%d", 228);

  for (;;) {
    __asm__ __volatile__("wfi");
  }
}

__attribute__((section(".text.boot"))) __attribute__((naked)) void boot(void) {
  __asm__ __volatile__("mv sp, %[stack_top]\n"
                       "j kernel_main\n"
                       :
                       : [stack_top] "r"(__stack_top));
}
