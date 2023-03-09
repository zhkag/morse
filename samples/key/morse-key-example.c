#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include <morse.h>
#include "drv_common.h"
#include "shell.h"

#define USER_KEY GET_PIN(D, 10) // GET_PIN(H,4)

void irq_callback()
{
    morse_input(!rt_pin_read(USER_KEY));
}

int morse_key_example(void)
{
    rt_pin_mode(USER_KEY, PIN_MODE_INPUT_PULLUP);
    rt_pin_attach_irq(USER_KEY, PIN_IRQ_MODE_RISING_FALLING, irq_callback, RT_NULL);
    rt_pin_irq_enable(USER_KEY, PIN_IRQ_ENABLE);
    return 0;
}

INIT_APP_EXPORT(morse_key_example);
