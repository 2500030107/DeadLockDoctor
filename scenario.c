
#include "deadlockdoctor.h"
#include <stdio.h>
#include <string.h>

static void print_safety(const SystemState *s)
{
    int seq[MAX_PROCESSES];

    if (banker_safe(s, seq)) {
        printf("SYSTEM SAFE\n");
        printf("Safe sequence: ");
        for (int i = 0; i < MAX_PROCESSES; i++)
            printf("P%d%s", seq[i], i == MAX_PROCESSES - 1 ? "\n" : " -> ");
    } else {
        printf("SYSTEM UNSAFE\n");
    }
}

int run_scenario(const char *filename)
{
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        perror("Cannot open scenario file");
        return 0;
    }

    SystemState s;
    init_safe_state(&s);

    char cmd[32];
    int line = 0;

    printf("\n====================================\n");
    printf("SCENARIO: %s\n", filename);
    printf("====================================\n");

    while (fscanf(fp, "%31s", cmd) == 1) {
        line++;

        if (cmd[0] == '#') {
            char rest[256];
            fgets(rest, sizeof(rest), fp);
            continue;
        }

        if (strcmp(cmd, "RESET_SAFE") == 0) {
            init_safe_state(&s);
            printf("\nState reset to SAFE example.\n");
        }
        else if (strcmp(cmd, "RESET_DEADLOCK") == 0) {
            init_deadlock_state(&s);
            printf("\nState reset to DEADLOCK example.\n");
        }
        else if (strcmp(cmd, "PRINT") == 0) {
            print_state(&s);
        }
        else if (strcmp(cmd, "SAFE_CHECK") == 0) {
            print_safety(&s);
        }
        else if (strcmp(cmd, "REQUEST") == 0) {
            int p, req[MAX_RESOURCES];
            if (fscanf(fp, "%d %d %d %d",
                       &p, &req[0], &req[1], &req[2]) != 4) {
                printf("Line %d: invalid REQUEST.\n", line);
                break;
            }

            printf("\nP%d requests: %d %d %d\n",
                   p, req[0], req[1], req[2]);
            request_resources(&s, p, req);
        }
        else if (strcmp(cmd, "RELEASE") == 0) {
            int p, rel[MAX_RESOURCES];
            if (fscanf(fp, "%d %d %d %d",
                       &p, &rel[0], &rel[1], &rel[2]) != 4) {
                printf("Line %d: invalid RELEASE.\n", line);
                break;
            }

            release_resources(&s, p, rel);
        }
        else if (strcmp(cmd, "DETECT") == 0) {
            int graph[MAX_PROCESSES][MAX_PROCESSES];
            int deadlocked[MAX_PROCESSES];

            build_wait_for_graph(&s, graph);
            print_wait_for_graph(graph);

            if (detect_deadlock(graph, deadlocked)) {
                printf("DEADLOCK DETECTED. Deadlocked set: ");
                for (int p = 0; p < MAX_PROCESSES; p++)
                    if (deadlocked[p]) printf("P%d ", p);
                printf("\n");
            } else {
                printf("NO DEADLOCK: no cycle found.\n");
            }
        }
        else if (strcmp(cmd, "RECOVER") == 0) {
            int victim;
            if (fscanf(fp, "%d", &victim) != 1) {
                printf("Line %d: invalid RECOVER.\n", line);
                break;
            }
            recover_victim(&s, victim);
        }
        else {
            char rest[256];
            fgets(rest, sizeof(rest), fp);
            printf("Line %d: unknown command '%s'.\n", line, cmd);
        }
    }

    fclose(fp);
    return 1;
}
