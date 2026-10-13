
#include "deadlockdoctor.h"
#include <stdio.h>
#include <string.h>

static int leq(const int a[MAX_RESOURCES], const int b[MAX_RESOURCES])
{
    for (int r = 0; r < MAX_RESOURCES; r++)
        if (a[r] > b[r]) return 0;
    return 1;
}

void calculate_need(SystemState *s)
{
    for (int p = 0; p < MAX_PROCESSES; p++)
        for (int r = 0; r < MAX_RESOURCES; r++)
            s->need[p][r] = s->maximum[p][r] - s->allocation[p][r];
}

void init_safe_state(SystemState *s)
{
    memset(s, 0, sizeof(*s));

    int av[MAX_RESOURCES] = {1, 1, 1};
    memcpy(s->available, av, sizeof(av));

    int a[MAX_PROCESSES][MAX_RESOURCES] = {
        {1, 0, 0},
        {0, 1, 0},
        {0, 0, 0},
        {0, 0, 0}
    };
    int m[MAX_PROCESSES][MAX_RESOURCES] = {
        {1, 1, 1},
        {1, 1, 1},
        {1, 1, 1},
        {1, 1, 1}
    };

    memcpy(s->allocation, a, sizeof(a));
    memcpy(s->maximum, m, sizeof(m));
    calculate_need(s);
}

void init_deadlock_state(SystemState *s)
{
    memset(s, 0, sizeof(*s));

    int av[MAX_RESOURCES] = {0, 0, 1};
    memcpy(s->available, av, sizeof(av));

    int a[MAX_PROCESSES][MAX_RESOURCES] = {
        {1, 0, 0},   /* P0 holds R0 */
        {0, 1, 0},   /* P1 holds R1 */
        {0, 0, 1},   /* P2 holds R2 */
        {0, 0, 0}
    };
    int m[MAX_PROCESSES][MAX_RESOURCES] = {
        {1, 1, 0},
        {1, 1, 0},
        {0, 0, 1},
        {0, 0, 0}
    };

    memcpy(s->allocation, a, sizeof(a));
    memcpy(s->maximum, m, sizeof(m));
    calculate_need(s);
}

void print_state(const SystemState *s)
{
    printf("\nAvailable: ");
    for (int r = 0; r < MAX_RESOURCES; r++) printf("%d ", s->available[r]);

    printf("\n\nAllocation:\n");
    for (int p = 0; p < MAX_PROCESSES; p++) {
        printf("P%d: ", p);
        for (int r = 0; r < MAX_RESOURCES; r++)
            printf("%d ", s->allocation[p][r]);
        printf("\n");
    }

    printf("\nNeed:\n");
    for (int p = 0; p < MAX_PROCESSES; p++) {
        printf("P%d: ", p);
        for (int r = 0; r < MAX_RESOURCES; r++)
            printf("%d ", s->need[p][r]);
        printf("\n");
    }
}

int banker_safe(const SystemState *s, int sequence[MAX_PROCESSES])
{
    int work[MAX_RESOURCES];
    int finish[MAX_PROCESSES] = {0};
    int count = 0;

    for (int r = 0; r < MAX_RESOURCES; r++)
        work[r] = s->available[r];

    while (count < MAX_PROCESSES) {
        int found = 0;

        for (int p = 0; p < MAX_PROCESSES; p++) {
            if (finish[p]) continue;

            if (leq(s->need[p], work)) {
                for (int r = 0; r < MAX_RESOURCES; r++)
                    work[r] += s->allocation[p][r];

                finish[p] = 1;
                sequence[count++] = p;
                found = 1;
            }
        }

        if (!found) break;
    }

    return count == MAX_PROCESSES;
}

int request_resources(SystemState *s, int pid,
                      const int request[MAX_RESOURCES])
{
    if (pid < 0 || pid >= MAX_PROCESSES) return 0;

    if (!leq(request, s->need[pid])) {
        printf("BLOCKED: request is greater than P%d's remaining Need.\n", pid);
        return 0;
    }

    if (!leq(request, s->available)) {
        printf("BLOCKED: resources are not currently available.\n");
        for (int r = 0; r < MAX_RESOURCES; r++)
            s->request[pid][r] = request[r];
        s->waiting[pid] = 1;
        return 0;
    }

    for (int r = 0; r < MAX_RESOURCES; r++) {
        s->available[r] -= request[r];
        s->allocation[pid][r] += request[r];
        s->need[pid][r] -= request[r];
    }

    int seq[MAX_PROCESSES];
    if (banker_safe(s, seq)) {
        s->waiting[pid] = 0;
        memset(s->request[pid], 0, sizeof(s->request[pid]));
        printf("GRANTED: system remains SAFE.\n");
        printf("Safe sequence: ");
        for (int i = 0; i < MAX_PROCESSES; i++)
            printf("P%d%s", seq[i], i == MAX_PROCESSES - 1 ? "\n" : " -> ");
        return 1;
    }

    for (int r = 0; r < MAX_RESOURCES; r++) {
        s->available[r] += request[r];
        s->allocation[pid][r] -= request[r];
        s->need[pid][r] += request[r];
    }

    s->waiting[pid] = 1;
    memcpy(s->request[pid], request, sizeof(s->request[pid]));
    printf("BLOCKED: request would make the system UNSAFE.\n");
    return 0;
}

void release_resources(SystemState *s, int pid,
                       const int release[MAX_RESOURCES])
{
    if (pid < 0 || pid >= MAX_PROCESSES) return;

    for (int r = 0; r < MAX_RESOURCES; r++) {
        int x = release[r];
        if (x > s->allocation[pid][r])
            x = s->allocation[pid][r];

        s->allocation[pid][r] -= x;
        s->need[pid][r] += x;
        s->available[r] += x;
    }

    s->waiting[pid] = 0;
    memset(s->request[pid], 0, sizeof(s->request[pid]));
    printf("RELEASED: P%d released resources.\n", pid);
}

void build_wait_for_graph(const SystemState *s,
                          int graph[MAX_PROCESSES][MAX_PROCESSES])
{
    memset(graph, 0, sizeof(int) * MAX_PROCESSES * MAX_PROCESSES);

    for (int p = 0; p < MAX_PROCESSES; p++) {
        if (!s->waiting[p]) continue;

        for (int r = 0; r < MAX_RESOURCES; r++) {
            if (s->request[p][r] <= 0) continue;

            for (int owner = 0; owner < MAX_PROCESSES; owner++) {
                if (owner != p && s->allocation[owner][r] > 0)
                    graph[p][owner] = 1;
            }
        }
    }
}

static int dfs(int u,
               const int graph[MAX_PROCESSES][MAX_PROCESSES],
               int state[MAX_PROCESSES],
               int deadlocked[MAX_PROCESSES])
{
    state[u] = 1;

    for (int v = 0; v < MAX_PROCESSES; v++) {
        if (!graph[u][v]) continue;

        if (state[v] == 0) {
            if (dfs(v, graph, state, deadlocked))
                return 1;
        } else if (state[v] == 1) {
            deadlocked[u] = 1;
            deadlocked[v] = 1;
            return 1;
        }
    }

    state[u] = 2;
    return 0;
}

int detect_deadlock(const int graph[MAX_PROCESSES][MAX_PROCESSES],
                    int deadlocked[MAX_PROCESSES])
{
    int state[MAX_PROCESSES] = {0};
    memset(deadlocked, 0, sizeof(int) * MAX_PROCESSES);

    for (int p = 0; p < MAX_PROCESSES; p++) {
        if (state[p] == 0 && dfs(p, graph, state, deadlocked))
            return 1;
    }

    return 0;
}

void print_wait_for_graph(const int graph[MAX_PROCESSES][MAX_PROCESSES])
{
    printf("\nWait-for Graph:\n");
    int any = 0;

    for (int p = 0; p < MAX_PROCESSES; p++) {
        for (int q = 0; q < MAX_PROCESSES; q++) {
            if (graph[p][q]) {
                printf("P%d -> P%d\n", p, q);
                any = 1;
            }
        }
    }

    if (!any) printf("No waiting edges.\n");
}

void recover_victim(SystemState *s, int victim)
{
    if (victim < 0 || victim >= MAX_PROCESSES) return;

    printf("\nRecovery: selecting P%d as victim.\n", victim);

    for (int r = 0; r < MAX_RESOURCES; r++) {
        s->available[r] += s->allocation[victim][r];
        s->allocation[victim][r] = 0;
        s->need[victim][r] = s->maximum[victim][r];
        s->request[victim][r] = 0;
    }

    s->waiting[victim] = 0;
    printf("P%d resources released.\n", victim);
}
