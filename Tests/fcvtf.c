#include "fcvtf.h"

#include <stdio.h>

/*
 * Standard library printf() does not support %f format specifier
 * in this environment, so we use a legacy function to print floats.
 */
int fcvtfprintf(float d, int ndigit) {
  extern char *fcvtf(float d, int ndigit, int *decpt, int *sign);
  int decpt, sign;
  const char *buf = fcvtf(d, ndigit, &decpt, &sign);
  return printf("%s%.*s.%s", sign ? "-" : "", decpt, buf, buf + decpt);
}
