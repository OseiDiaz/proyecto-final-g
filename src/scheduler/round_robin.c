#include "scheduler.h"

#include <stdlib.h>
#include <stdio.h>

#include "../../include/scheduler.h"
#include "../../include/circular_queue.h"

typedef struct {
    int pid;
    int burst_time;
    int remaining_time;
    int state;
} RRProcess;

struct RoundRobinScheduler {
    CircularQueue* queue;
    RRProcess* processes;
    int capacity;
    int count;
    int quantum;
    int current_time;
};

RoundRobinScheduler* rr_create(
    int capacity,
    int quantum
) {
    RoundRobinScheduler* scheduler =
        malloc(sizeof(RoundRobinScheduler));

    if (scheduler == NULL)
        return NULL;

    scheduler->queue = cq_create(capacity);
    scheduler->processes = malloc(sizeof(RRProcess) * capacity);

    if (scheduler->queue == NULL || scheduler->processes == NULL) {
        cq_destroy(scheduler->queue);
        free(scheduler->processes);
        free(scheduler);
        return NULL;
    }

    scheduler->capacity = capacity;
    scheduler->count = 0;
    scheduler->quantum = quantum;
    scheduler->current_time = 0;

    return scheduler;
}

void rr_add_process(
    RoundRobinScheduler* scheduler,
    int pid,
    int burst_time
) {
    if (scheduler == NULL)
        return;

    if (scheduler->count >= scheduler->capacity)
        return;

    int index = scheduler->count;

    scheduler->processes[index].pid = pid;
    scheduler->processes[index].burst_time = burst_time;
    scheduler->processes[index].remaining_time = burst_time;
    scheduler->processes[index].state = 0;

    cq_enqueue(scheduler->queue, index);

    scheduler->count++;
}

int rr_next(
    RoundRobinScheduler* scheduler
) {
    if (scheduler == NULL)
        return -1;

    if (cq_is_empty(scheduler->queue))
        return -1;

    int index = cq_dequeue(scheduler->queue);

    RRProcess* process =
        &scheduler->processes[index];

    process->state = 1;

    int execution_time;

    if (process->remaining_time > scheduler->quantum)
        execution_time = scheduler->quantum;
    else
        execution_time = process->remaining_time;

    process->remaining_time -= execution_time;
    scheduler->current_time += execution_time;

    printf(
        "Tiempo %d: Proceso %d ejecuta %d unidades. Restante: %d\n",
        scheduler->current_time,
        process->pid,
        execution_time,
        process->remaining_time
    );

    if (process->remaining_time > 0) {
        process->state = 0;
        cq_enqueue(scheduler->queue, index);
    } else {
        process->state = 2;
    }

    return process->pid;
}

int rr_is_finished(
    RoundRobinScheduler* scheduler
) {
    if (scheduler == NULL)
        return 1;

    return cq_is_empty(scheduler->queue);
}

void rr_destroy(
    RoundRobinScheduler* scheduler
) {
    if (scheduler == NULL)
        return;

    cq_destroy(scheduler->queue);
    free(scheduler->processes);
    free(scheduler);
}
