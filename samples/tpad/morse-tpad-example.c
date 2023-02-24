

#include <rtthread.h>
#include <board.h>
#include <morse.h>
#include <tpad.h>

void morse_input_entry(void *parameter)
{
    while (1)
    {
        morse_input(tpad_scan(1));
        rt_thread_mdelay(10);
    }
}

int morse_tpad_example(void)
{
    rt_thread_t tid;
    tpad_init(8);                           /* 初始化触摸按键 */
    morse_init();
    tid = rt_thread_create("morse_tpad_example", morse_input_entry, RT_NULL, 4096, 20, 10);

    if (tid != NULL)
        rt_thread_startup(tid);
    return RT_EOK;
}

INIT_APP_EXPORT(morse_tpad_example);
