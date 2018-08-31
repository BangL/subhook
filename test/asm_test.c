#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <subhook.h>

typedef void (*foo_func_t)();

void foo();
foo_func_t foo_tr = NULL;

void foo_hooked() {
  puts("foo_hooked() called");
}

void foo_hooked_tr() {
  puts("foo_hooked_tr() called");
  foo_tr();
}

int main() {
  puts("Testing initial install");

  subhook_t foo_hook = subhook_new((void *)foo,
                                   (void *)foo_hooked,
                                   SUBHOOK_OPTION_64BIT_OFFSET);
  if (foo_hook == NULL || subhook_install(foo_hook) < 0) {
    puts("Install failed");
    return EXIT_FAILURE;
  }
  foo();
  if (subhook_remove(foo_hook) < 0) {
    puts("Remove failed");
    return EXIT_FAILURE;
  }
  foo();

  puts("Testing re-install");

  if (subhook_install(foo_hook) < 0) {
    puts("Install failed");
    return EXIT_FAILURE;
  }
  foo();
  if (subhook_remove(foo_hook) < 0) {
    puts("Remove failed");
    return EXIT_FAILURE;
  }
  foo();

  puts("Testing trampoline");

  subhook_t foo_hook_tr = subhook_new((void *)foo,
                                      (void *)foo_hooked_tr,
                                      SUBHOOK_OPTION_64BIT_OFFSET);
  if (subhook_install(foo_hook_tr) < 0) {
    puts("Install failed");
    return EXIT_FAILURE;
  }
  foo_tr = (foo_func_t)subhook_get_trampoline(foo_hook_tr);
  if (foo_tr == NULL) {
    puts("Failed to build trampoline");
    return EXIT_FAILURE;
  }
  foo();

  return EXIT_SUCCESS;
}
