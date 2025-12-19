#include "monitor_handles.h"

#include <stdio.h>
#include <unistd.h>

int main(void) {
  initialise_monitor_handles();
  (void)printf("Hello, World from a_test!\n");
  _exit(0);
  return 0;
}
