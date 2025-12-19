#include "fcvtf.h"

#include <stdio.h>

int fcvtfprintf(float d, int ndigit) {
  extern char *fcvtf(float d, int ndigit, int *decpt, int *sign);
  int decpt, sign;
  const char *buf = fcvtf(d, ndigit, &decpt, &sign);
  return printf("%s%.*s.%s", sign ? "-" : "", decpt, buf, buf + decpt);
}
