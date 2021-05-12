/*
 * This code is licensed under MIT license (see LICENSE for details)
 * (c) 2021 Andrew V. Jones
 */

#include <stddef.h>

void crash() {
  int *p = NULL;
  *p = 10;
}

// EOF
