#ifndef __MORSE_H__
#define __MORSE_H__
#include <rtthread.h>

void dida_add(rt_bool_t flag);
void dida_clean();
char morse_analysis(rt_uint8_t value);
static void morse_timeout(void *parameter);
void morse_timer_start(rt_int32_t ms);
void morse_input(rt_bool_t flag);
void morse_init();

#endif
