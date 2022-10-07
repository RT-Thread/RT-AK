#ifndef __SYSTICK_H
#define __SYSTICK_H

#include <stdint.h>

void systick_sleep(int ms);
uint32_t systick_current_millis(void);

#endif
