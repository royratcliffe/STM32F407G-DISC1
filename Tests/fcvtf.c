#include "fcvtf.h"

#include <malloc.h>
#include <stdio.h>
#include <string.h>

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

char *cvtfbuf(float f, int ndigit, char *buf) {
  /*
   * Allocate buffer space from the heap if not provided by the user. As a
   * reasonable estimate, ask for ndigit + 36 characters to accommodate the
   * digits, decimal point, sign, and possible leading "0." and extra zeros.
   * Remember to free the buffer after use if it was allocated here.
   */
  if (buf == NULL) {
    if ((buf = (char *)realloc(buf, ndigit + 36)) == NULL)
      return NULL;
  }
  int decpt, sign;
  const char *cvt = fcvtf(f, ndigit, &decpt, &sign);
  char *ptr = buf;
  if (sign) {
    *ptr++ = '-';
  }
  /*
   * Construct the final part of the string. If the decimal point is before the
   * first digit, insert a leading "0." and the appropriate number of zeros.
   * Otherwise, copy the digits before the decimal point,
   */
  if (decpt <= 0) {
    *ptr++ = '0';
    *ptr++ = '.';
    for (int i = 0; i < -decpt; i++) {
      *ptr++ = '0';
    }
    (void)strcpy(ptr, cvt);
  } else {
    (void)strncpy(ptr, cvt, decpt);
    ptr += decpt;
    *ptr++ = '.';
    (void)strcpy(ptr, cvt + decpt);
  }
  return buf;
}
