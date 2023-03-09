#ifndef __MORSE_H__
#define __MORSE_H__
#include <rtthread.h>

#ifndef PKG_USING_MORSE_DI_DA_SEPARATE
#define PKG_USING_MORSE_DI_DA_SEPARATE    150
#endif

#ifndef PKG_USING_MORSE_INTERCHAR
#define PKG_USING_MORSE_INTERCHAR    300
#endif

void dida_add(rt_bool_t flag);
void dida_clean();
char morse_analysis(rt_uint16_t value);
void morse_timeout(void *parameter);
void morse_timer_start(rt_int32_t ms);
void morse_input(rt_bool_t flag);
int morse_init();

#ifdef PKG_USING_MORSE_SHELL
rt_uint8_t morse_getc();
#endif

#endif
