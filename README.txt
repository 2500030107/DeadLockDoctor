DEADLOCKDOCTOR - COMPLETE SIMPLE C PROJECT
===========================================

Programming Language:
    C (C11 standard)

Operating System:
    Linux / Ubuntu

Main Features:
    1. Multi-instance resource management
    2. Allocation, Max, Need and Available matrices
    3. Banker's Algorithm safety check
    4. Scenario-file runner
    5. Wait-for graph construction
    6. DFS cycle detection
    7. Deadlock recovery by victim selection
    8. Live pthread mutex deadlock demonstration

COMPILE
-------
gcc -std=c11 -Wall -Wextra -pedantic -O2 \
    main.c deadlockdoctor.c scenario.c live_demo.c \
    -o deadlockdoctor -pthread

RUN BUILT-IN DEMO
-----------------
./deadlockdoctor

RUN SAFE SCENARIO
-----------------
./deadlockdoctor scenarios/safe.scn

RUN DEADLOCK SCENARIO
---------------------
./deadlockdoctor scenarios/deadlock.scn

MAKE COMMANDS
-------------
make
make safe
make deadlock
make clean

LIVE THREAD DEMO
-----------------
The live demo uses two pthreads:
    Thread 1: locks A, then waits for B
    Thread 2: locks B, then waits for A

The manager observes:
    Thread 1 -> Thread 2 -> Thread 1

The demo uses timed mutex waits so the program does not hang forever.

To enable it:
    1. Open main.c.
    2. Add:
           live_thread_demo();
       before return 0; in main().
    3. Recompile.

NOTE
----
The project specification also mentions optional features:
    - live text visualization of the resource-allocation graph
    - prevention strategies
    - timeout-based detection
These can be added later if your team actually implements them.
