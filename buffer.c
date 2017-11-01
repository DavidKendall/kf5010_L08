#include <assert.h>
#include <semaphore.h>
#include "buffer.h"

static sem_t full_slot;
static sem_t free_slot;
static sem_t buf_mutex;

static message_t buffer[BUF_SIZE];
static int front = 0;
static int back = 0;

void put_buffer(message_t const * const msg) {
  buffer[back] = *msg;
  back = (back + 1) % BUF_SIZE;
}

void get_buffer(message_t * const msg) {
  *msg = buffer[front];
  front = (front + 1) % BUF_SIZE;
}

void safe_buffer_init(void) {
  int rc;

  rc = sem_init(&full_slot, 0, 0);
  assert(rc == 0);
  rc = sem_init(&free_slot, 0, BUF_SIZE);
  assert(rc == 0);
  rc = sem_init(&buf_mutex, 0, 1);
  assert(rc == 0);
}

void safe_buffer_put(message_t const * const msg) {
  int rc;

  rc = sem_wait(&free_slot);
  assert(rc == 0);
  rc = sem_wait(&buf_mutex);
  assert(rc == 0);
  put_buffer(msg);
  rc = sem_post(&buf_mutex);
  assert(rc == 0);
  rc = sem_post(&full_slot);
  assert(rc == 0);
}

void safe_buffer_get(message_t * const msg) {
  int rc;

  rc = sem_wait(&full_slot);
  assert(rc == 0);
  rc = sem_wait(&buf_mutex);
  assert(rc == 0);
  get_buffer(msg);
  rc = sem_post(&buf_mutex);
  assert(rc == 0);
  rc = sem_post(&free_slot);
  assert(rc == 0);
}

