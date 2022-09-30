#include "mutex.h"

void mutex_init(mutex_t *mutex)
{
    mutex->lock = 0;
    mutex->tid = 0;
}

void mutex_lock(mutex_t *mutex, uint32_t tid)
{
    //todo not use
}

int mutex_try_lock(mutex_t *mutex, uint32_t tid)
{
    // If mutex is already locked by the current thread then
    // release the Kraken err.. the mutex, else attempt to lock it.
    if (mutex->tid == tid)
    {
        mutex_unlock(mutex, tid);
    }
    else if (mutex->lock == 0)
    {
        mutex->lock = 1;
        mutex->tid = tid;
        return 1;
    }

    return 0;
}

void mutex_unlock(mutex_t *mutex, uint32_t tid)
{
    if (mutex->tid == tid)
    {
        mutex->lock = 0;
        mutex->tid = 0;
    }
}
