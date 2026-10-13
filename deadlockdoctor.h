
#ifndef DEADLOCKDOCTOR_H
#define DEADLOCKDOCTOR_H

#define MAX_PROCESSES 4
#define MAX_RESOURCES 3

typedef struct {
    int available[MAX_RESOURCES];
    int allocation[MAX_PROCESSES][MAX_RESOURCES];
    int maximum[MAX_PROCESSES][MAX_RESOURCES];
    int need[MAX_PROCESSES][MAX_RESOURCES];
    int waiting[MAX_PROCESSES];
    int request[MAX_PROCESSES][MAX_RESOURCES];
} SystemState;

void init_safe_state(SystemState *s);
void init_deadlock_state(SystemState *s);
void calculate_need(SystemState *s);
void print_state(const SystemState *s);

int banker_safe(const SystemState *s, int sequence[MAX_PROCESSES]);
int request_resources(SystemState *s, int pid, const int request[MAX_RESOURCES]);
void release_resources(SystemState *s, int pid, const int release[MAX_RESOURCES]);

void build_wait_for_graph(const SystemState *s,
                          int graph[MAX_PROCESSES][MAX_PROCESSES]);
int detect_deadlock(const int graph[MAX_PROCESSES][MAX_PROCESSES],
                    int deadlocked[MAX_PROCESSES]);
void print_wait_for_graph(const int graph[MAX_PROCESSES][MAX_PROCESSES]);

void recover_victim(SystemState *s, int victim);

int run_scenario(const char *filename);
void live_thread_demo(void);

#endif
