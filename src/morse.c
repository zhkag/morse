#include <morse.h>

rt_tick_t millisecond = 0;
rt_bool_t morse_flag = RT_FALSE;
rt_uint8_t morse_value = 0x01;

char morse_array[255] = {0,0,'e','t','i','a','n','m','s','u','r','w','d','k',
        'g','o','h','v','f',0,'l',0,'p','j','b','x','c','y','z','q',0,0,
        '5','4',0,'3',0,0,0,'2',0,0,0,0,0,'1','6',0,0,0,0,0,0,0,'7',0,0,0,'8',0,'9','0'};

#define RT_DA 1
#define RT_DI 0

static rt_timer_t morse_timer;
static void morse_timeout(void *parameter)
{
    rt_kprintf("%c",morse_analysis(morse_value));
    dida_clean();
}

void morse_timer_start(rt_int32_t ms)
{
    rt_tick_t tick = rt_tick_from_millisecond(ms);
    if (morse_timer != RT_NULL)
    {
        rt_timer_control(morse_timer, RT_TIMER_CTRL_SET_TIME, &tick);
        rt_timer_start(morse_timer);
    }
}

void dida_add(rt_bool_t flag)
{
    morse_value = (morse_value << 1) | flag;
}

void dida_clean()
{
    morse_value = 0x01;
}

char morse_analysis(rt_uint8_t value)
{
    return morse_array[value];
}

void morse_input(rt_bool_t flag)
{
    if(morse_flag != flag)
    {
        rt_tick_t interval = rt_tick_get_millisecond() - millisecond;
        millisecond = rt_tick_get_millisecond();
        if(flag)
        {
            rt_timer_stop(morse_timer);
        }
        else
        {
            if(interval <= 150)
                dida_add(RT_DI);
            else{
                dida_add(RT_DA);
            }
            morse_timer_start(300);
        }
        morse_flag = flag;
    }
}

void morse_init()
{
    morse_timer = rt_timer_create("morse", morse_timeout,
                             RT_NULL,  0,
                             RT_TIMER_FLAG_ONE_SHOT);
}
