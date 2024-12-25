#ifndef _QUEUE_H_
#define _QUEUE_H_

typedef struct Task Task_t;
struct Task {
    int         id;
    int         burst_time;
    int         remaining_burst_time;
    int         current_queue;

    int         total_wait_time;        // For Computing `Wait Time`
    int         total_execution_time;   // For Computing `Turn Around Time`
    Task_t      *next;                  // For Queue (Linked List) Operations
};

typedef struct Instruction Instruction_t;
struct Instruction {
    int         event_tick;
    int         task_id;
    int         burst_time;
    int         is_eof;
};

typedef struct Queue Queue_t;
struct Queue {
    Task_t *start;
    Task_t *end;
};

Queue_t *init_queue();
int is_empty(Queue_t *);

void enqueue(Queue_t *, Task_t *);
Task_t *dequeue(Queue_t *);

void deallocate(void *);

#endif