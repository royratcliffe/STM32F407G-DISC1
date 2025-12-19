#include "monitor_handles.h"
#include "ring_buf.h"

#include <assert.h>
#include <stdio.h>
#include <unistd.h>

int main(void) {
  initialise_monitor_handles();
  (void)printf("Hello, World from %s!!!\n", "a_test");

  RING_BUF_DEFINE(buf, sizeof(float[4U]));
  float number = 1.0F;
  ring_buf_size_t ack;
  while ((ack = ring_buf_put(&buf, &number, sizeof(number)))) {
    int err = ring_buf_put_ack(&buf, ack);
    assert(err == 0);
    number += 1.0F;
  }
  float sum = 0.0F;
  void *space;
  while ((ring_buf_get_claim(&buf, &space, sizeof(float))))
    sum += *(float *)space;
  assert(sum == 1.0F + 2.0F + 3.0F + 4.0F);
  assert(ring_buf_free_space(&buf) == 0U);
  ring_buf_get_ack(&buf, 0U);
  assert(ring_buf_used_space(&buf) == sizeof(float[4U]));
  while ((ack = ring_buf_get(&buf, &number, sizeof(number)))) {
    int err = ring_buf_get_ack(&buf, ack);
    assert(err == 0);
    (void)printf("number=%d\n", (int)number);
  }
  assert(ring_buf_free_space(&buf) == sizeof(float[4U]));
  assert(ring_buf_used_space(&buf) == 0U);

  _exit(0);
  return 0;
}
