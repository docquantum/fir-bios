/*
 * Daniel Shchur
 * Queue object type in C
 *
 * Maintains a basic queue for audio data
 *
 */

#include "queue.h"

static int16_t sampleQueue[Q_MAX_SIZE];
static int16_t head = 0;
static int16_t tail = 0;
static int16_t size = 0;

int16_t q_pop()
{
    int16_t popped = 0;
    if (size == 0)
        return popped;
    popped = sampleQueue[head];
    head++;
    if (head >= Q_MAX_SIZE)
        head = 0;
    size--;
    return popped;
}

int16_t q_push(int16_t sample)
{
    if (size == Q_MAX_SIZE)
        return 0;
    tail++;
    if (tail >= Q_MAX_SIZE)
        tail = 0;
    sampleQueue[tail] = sample;
    size++;
    return 1;
}

void q_empty()
{
    head = 0;
    tail = 0;
    size = 0;
}

int16_t q_is_empty()
{
    return size == 0;
}

int16_t q_is_full()
{
    return size == Q_MAX_SIZE;
}
