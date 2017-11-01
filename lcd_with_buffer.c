#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <assert.h>
#include <pthread.h>
#include <semaphore.h>

#include "buffer.h"
#include "console.h"

void *lcd_thr(void *arg);
void *count_thr(void *arg);

int main (void) {
    pthread_t thread[4];
    int rc;
    unsigned long i;

    console_init();
    safe_buffer_init();

    rc = pthread_create(&thread[0], NULL, lcd_thr, NULL);
    assert(rc == 0);

    for (i = 1; i < 3; i += 1) {
        rc = pthread_create(&thread[i], NULL, count_thr, (void *)i);
        assert(rc == 0);
    }

    while (true) {
        /* skip */
    }
    exit(0);
}

void *count_thr(void *arg) {
    unsigned long id = (long)arg;
    unsigned long counter = 0;
    message_t msg;

    msg.id = id;
    while (true) {
        counter += 1;
        msg.counter = counter;
        safe_buffer_put(&msg);
    }
}

void *lcd_thr(void * arg) {
    message_t msg;

    while (true) {
        safe_buffer_get(&msg);
        lcd_write_at(msg.id, 0, "Thread %ld counter is %10ld", msg.id, msg.counter);
    }
}

