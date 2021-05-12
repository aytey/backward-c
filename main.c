/*
 * This code is licensed under MIT license (see LICENSE for details)
 * (c) 2021 Andrew V. Jones
 */

#include <signal.h>
#include <stdlib.h>

/* some code that crashes */
extern void crash(void);

/* prototype for show_backtrace */
extern void show_backtrace(void);

void handler(int sig) {
  /* silence unused parameter */
  (void)sig;

  /* show a backtrace */
  show_backtrace();

  /* teardown our application */
  exit(1);
}

void setup_handlers(void) {
  signal(SIGSEGV, handler);
  signal(SIGABRT, handler);
  signal(SIGBUS, handler);
  signal(SIGILL, handler);
  signal(SIGFPE, handler);
}

int main(void) {
  setup_handlers();
  crash();
  return 0;
}

// EOF
