/*
 * queue.c
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

/*
 * Wrapper around malloc() -- ends the program if malloc() fails
 * (i.e., eliminates the need of an "if" statement around each
 * call to malloc() in the main body of code).
 */
void *emalloc(size_t n) {
    void *p;

    p = malloc(n);
    if (p == NULL) {
        fprintf(stderr, "malloc of %zu bytes failed", n);
        exit(1);
    }

    return p;
}

/*
 * Initialize an empty Queue. 
 */
Queue_t *init_queue() {
    Queue_t* q = (Queue_t*) emalloc(sizeof(Queue_t));
    q->start = NULL;
    q->end = NULL;

    return q;
}

/*
 * Check if the queue is empty. 
 */
int is_empty(Queue_t* q){
    if(q->start == NULL) {
        return 1;
    }

    return 0;
}

/*
 * Wrapper to free allocated memory.
 */
void deallocate(void *t) {
    assert (t != NULL);
    free(t);
}

/*
 * At the end of this operation, the node passed in will be enqueued. 
 */
void enqueue(Queue_t *q, Task_t *task) {
    // task->next = NULL;

    if (is_empty(q)) {
        q->start = task;
        q->end = task;

    } else {
        q->end->next = task;
        q->end = task;
    }
}

/*
 * Returns and dequeues the start node from the queue.
 * Deallocate/free the Node after use.
 */
Task_t *dequeue(Queue_t *q) {
    Task_t *task;

    if (is_empty(q)) {
        return NULL;
    }
    task = q->start;
    q->start = task->next;

    if(q->start == NULL) {
        q->end = NULL;
    }

    task->next = NULL;
    return task;
}