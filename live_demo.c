
#define _POSIX_C_SOURCE 200809L

#include "deadlockdoctor.h"
#include <pthread.h>
#include <stdio.h>
#include <time.h>

static pthread_mutex_t lock_a = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t lock_b = PTHREAD_MUTEX_INITIALIZER;

static volatile int t1_holds_a = 0;
static volatile int t1_waits_b = 0;
static volatile int t2_holds_b = 0;
static volatile int t2_waits_a = 0;

static int try_lock_for(pthread_mutex_t *m, int seconds)
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += seconds;
    return pthread_mutex_timedlock(m, &ts);
}

static void *thread1(void *arg)
{
    (void)arg;

    pthread_mutex_lock(&lock_a);
    t1_holds_a = 1;
    printf("Thread 1: locked A\n");

    struct timespec delay = {0, 200000000};
    nanosleep(&delay, NULL);

    t1_waits_b = 1;
    printf("Thread 1: waiting for B\n");

    if (try_lock_for(&lock_b, 2) == 0) {
        printf("Thread 1: got B\n");
        pthread_mutex_unlock(&lock_b);
    } else {
        printf("Thread 1: could not get B (deadlock timeout)\n");
    }

    t1_waits_b = 0;
    t1_holds_a = 0;
    pthread_mutex_unlock(&lock_a);
    return NULL;
}

static void *thread2(void *arg)
{
    (void)arg;

    pthread_mutex_lock(&lock_b);
    t2_holds_b = 1;
    printf("Thread 2: locked B\n");

    struct timespec delay = {0, 200000000};
    nanosleep(&delay, NULL);

    t2_waits_a = 1;
    printf("Thread 2: waiting for A\n");

    if (try_lock_for(&lock_a, 2) == 0) {
        printf("Thread 2: got A\n");
        pthread_mutex_unlock(&lock_a);
    } else {
        printf("Thread 2: could not get A (deadlock timeout)\n");
    }

    t2_waits_a = 0;
    t2_holds_b = 0;
    pthread_mutex_unlock(&lock_b);
    return NULL;
}

void live_thread_demo(void)
{
    pthread_t a, b;

    printf("\n====================================\n");
    printf("LIVE THREAD DEADLOCK DEMO\n");
    printf("====================================\n");

    pthread_create(&a, NULL, thread1, NULL);
    pthread_create(&b, NULL, thread2, NULL);

    struct timespec observe_delay = {1, 0};
    nanosleep(&observe_delay, NULL);

    /*
     * Manager observes:
     * T1 holds A and waits for B.
     * T2 holds B and waits for A.
     * Therefore: T1 -> T2 -> T1.
     */
    if (t1_holds_a && t1_waits_b &&
        t2_holds_b && t2_waits_a) {
        printf("\nMANAGER: circular wait detected.\n");
        printf("MANAGER: Thread1 -> Thread2 -> Thread1\n");
        printf("MANAGER: DEADLOCK DETECTED\n");
    } else {
        printf("\nMANAGER: no circular wait observed.\n");
    }

    pthread_join(a, NULL);
    pthread_join(b, NULL);

    printf("LIVE DEMO FINISHED.\n");
}
