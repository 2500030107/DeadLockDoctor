#include "deadlockdoctor.h"
#include <stdio.h>

void user_input_demo()
{
    SystemState s;
    int seq[MAX_PROCESSES];
    int pid;
    int request[MAX_RESOURCES];

    int n, m;

    printf("\n====================================\n");
    printf("        DEADLOCK DOCTOR\n");
    printf("====================================\n");

    /* Number of processes and resources */
    printf("\nEnter number of processes (1-%d): ", MAX_PROCESSES);
    scanf("%d", &n);

    printf("Enter number of resources (1-%d): ", MAX_RESOURCES);
    scanf("%d", &m);

    if (n < 1 || n > MAX_PROCESSES ||
        m < 1 || m > MAX_RESOURCES) {

        printf("\nInvalid number of processes/resources.\n");
        return;
    }

    /* Available resources */
    printf("\nEnter Available resources (%d values):\n", m);

    for (int r = 0; r < m; r++) {
        scanf("%d", &s.available[r]);
    }

    /* Allocation matrix */
    printf("\nEnter Allocation Matrix (%d x %d):\n", n, m);

    for (int p = 0; p < n; p++) {

        printf("P%d: ", p);

        for (int r = 0; r < m; r++) {
            scanf("%d", &s.allocation[p][r]);
        }
    }

    /* Maximum matrix */
    printf("\nEnter Maximum Matrix (%d x %d):\n", n, m);

    for (int p = 0; p < n; p++) {

        printf("P%d: ", p);

        for (int r = 0; r < m; r++) {
            scanf("%d", &s.maximum[p][r]);
        }
    }

    /* Clear unused processes/resources */
    for (int p = n; p < MAX_PROCESSES; p++) {

        for (int r = 0; r < MAX_RESOURCES; r++) {
            s.allocation[p][r] = 0;
            s.maximum[p][r] = 0;
            s.need[p][r] = 0;
        }
    }

    for (int r = m; r < MAX_RESOURCES; r++) {
        s.available[r] = 0;
    }

    /* Calculate Need */
    calculate_need(&s);

    /* Initialize waiting/request information */
    for (int p = 0; p < MAX_PROCESSES; p++) {

        s.waiting[p] = 0;

        for (int r = 0; r < MAX_RESOURCES; r++) {
            s.request[p][r] = 0;
        }
    }

    printf("\n========== CURRENT STATE ==========\n");

    printf("\nAvailable: ");

    for (int r = 0; r < m; r++) {
        printf("%d ", s.available[r]);
    }

    printf("\n\nAllocation:\n");

    for (int p = 0; p < n; p++) {

        printf("P%d: ", p);

        for (int r = 0; r < m; r++) {
            printf("%d ", s.allocation[p][r]);
        }

        printf("\n");
    }

    printf("\nNeed:\n");

    for (int p = 0; p < n; p++) {

        printf("P%d: ", p);

        for (int r = 0; r < m; r++) {
            printf("%d ", s.need[p][r]);
        }

        printf("\n");
    }

    /* Safety check */
    printf("\n========== SAFETY CHECK ==========\n");

    if (banker_safe(&s, seq)) {

        printf("SYSTEM SAFE\n");

        printf("Safe sequence: ");

        for (int i = 0; i < n; i++) {

            printf("P%d", seq[i]);

            if (i < n - 1)
                printf(" -> ");
        }

        printf("\n");

    } else {

        printf("SYSTEM UNSAFE\n");
    }

    /* Resource request */
    printf("\n========== RESOURCE REQUEST ==========\n");

    printf("Enter process number (0-%d): ", n - 1);
    scanf("%d", &pid);

    if (pid < 0 || pid >= n) {

        printf("Invalid process number.\n");
        return;
    }

    printf("Enter request (%d values): ", m);

    for (int r = 0; r < m; r++) {
        scanf("%d", &request[r]);
    }

    /* Clear unused request values */
    for (int r = m; r < MAX_RESOURCES; r++) {
        request[r] = 0;
    }

    printf("\nP%d requests: ", pid);

    for (int r = 0; r < m; r++) {
        printf("%d ", request[r]);
    }

    printf("\n");

    request_resources(&s, pid, request);

    /* Deadlock detection */
    printf("\n========== DEADLOCK DETECTION ==========\n");

    int graph[MAX_PROCESSES][MAX_PROCESSES];
    int deadlocked[MAX_PROCESSES];

    build_wait_for_graph(&s, graph);

    print_wait_for_graph(graph);

    if (detect_deadlock(graph, deadlocked)) {

        printf("DEADLOCK DETECTED\n");

        printf("Deadlocked processes: ");

        for (int p = 0; p < n; p++) {

            if (deadlocked[p])
                printf("P%d ", p);
        }

        printf("\n");

        printf("\nEnter victim process for recovery: ");
        scanf("%d", &pid);

        if (pid >= 0 && pid < n) {

            recover_victim(&s, pid);

            build_wait_for_graph(&s, graph);

            print_wait_for_graph(graph);

            printf("Recovery completed.\n");

        } else {

            printf("Invalid victim process.\n");
        }

    } else {

        printf("NO DEADLOCK DETECTED\n");
    }

    /* Final state */
    printf("\n========== FINAL STATE ==========\n");

    printf("\nAvailable: ");

    for (int r = 0; r < m; r++) {
        printf("%d ", s.available[r]);
    }

    printf("\n\nAllocation:\n");

    for (int p = 0; p < n; p++) {

        printf("P%d: ", p);

        for (int r = 0; r < m; r++) {
            printf("%d ", s.allocation[p][r]);
        }

        printf("\n");
    }

    printf("\nNeed:\n");

    for (int p = 0; p < n; p++) {

        printf("P%d: ", p);

        for (int r = 0; r < m; r++) {
            printf("%d ", s.need[p][r]);
        }

        printf("\n");
    }
}

int main()
{
    user_input_demo();

    return 0;
}
