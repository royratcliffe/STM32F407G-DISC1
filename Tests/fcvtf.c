#include "fcvtf.h"

#include <stdio.h>

extern char *fcvtf(float d, int ndigit, int *decpt, int *sign);

/*
 * Standard library printf() does not support %f format specifier
 * in this environment, so we use a legacy function to print floats.
 */
int fcvtfprintf(float d, int ndigit) {
  int decpt, sign;
  const char *buf = fcvtf(d, ndigit, &decpt, &sign);
  return printf("%s%.*s.%s", sign ? "-" : "", decpt, buf, buf + decpt);
}

const char *_fcvtf(float d, int ndigit) {
  static char str[80];
  int decpt, sign;
  const char *buf = fcvtf(d, ndigit, &decpt, &sign);
  (void)snprintf(str, sizeof(str), "%s%.*s.%s", sign ? "-" : "", decpt, buf, buf + decpt);
  return str;
}
