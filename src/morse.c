#include <morse.h>

#define RT_DA 1
#define RT_DI 0

static rt_timer_t morse_timer;
rt_tick_t millisecond = 0;
rt_bool_t morse_flag = RT_FALSE;
rt_uint16_t morse_value = 0x01;
rt_uint8_t morse_char = 0xff;
rt_uint8_t morse_num_max = 0;

typedef struct {
    rt_uint16_t id;
    rt_uint8_t value;
} morse_equivalent;

static const morse_equivalent morse_equivalent_type[] = {
        { 0b00010, 'e' }, { 0b00011, 't' }, { 0b00100, 'i' }, { 0b00101, 'a' }, { 0b00110, 'n' },
        { 0b00111, 'm' }, { 0b01000, 's' }, { 0b01001, 'u' }, { 0b01010, 'r' }, { 0b01011, 'w' },
        { 0b01100, 'd' }, { 0b01101, 'k' }, { 0b01110, 'g' }, { 0b01111, 'o' }, { 0b10000, 'h' },
        { 0b10001, 'v' }, { 0b10010, 'f' }, { 0b10100, 'l' }, { 0b10110, 'p' }, { 0b10111, 'j' },
        { 0b11000, 'b' }, { 0b11001, 'x' }, { 0b11010, 'c' }, { 0b11011, 'y' }, { 0b11100, 'z' },
        { 0b11101, 'q' },

        { 0b100000, '5' }, { 0b100001, '4' }, { 0b100011, '3' }, { 0b100111, '2' }, { 0b101000, '&' },
        { 0b101010, '+' }, { 0b101111, '1' }, { 0b110000, '6' }, { 0b110001, '=' }, { 0b110010, '/' },
        { 0b110110, '(' }, { 0b111000, '7' }, { 0b111100, '8' }, { 0b111110, '9' }, { 0b111111, '0' },

        { 0b1000101, '\n' }, { 0b1001100, '?' }, { 0b1001101, '_' }, { 0b1010010, '"' }, { 0b1010101, '.' }, { 0b1011010, '@' },
        { 0b1011110, '\'' }, { 0b1100001, '-' }, { 0b1101010, ';' }, { 0b1101011, '!' }, { 0b1101101, ')' }, { 0b1110011, ',' }, { 0b1111000, ':' },

        { 0b10001001, '$' }, { 0b100000000, '\b' },

        { 0b1, '\0' },
};

void morse_timeout(void *parameter)
{
    morse_char = morse_analysis(morse_value);
#ifndef PKG_USING_MORSE_SHELL
    rt_kprintf("%c",morse_char);
#endif
    dida_clean();
}

#ifdef PKG_USING_MORSE_SHELL
rt_uint8_t morse_getc()
{
    int ch;
    while (morse_char == 0xff)
    {
        rt_thread_mdelay(100);
    }
    ch = morse_char;
    morse_char = 0xff;
    return ch;
}
#endif

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

char morse_analysis(rt_uint16_t value)
{
    for (rt_uint8_t middle, start = 0, end = morse_num_max; start <= end;) {
        middle = (start + end) / 2;
        if(morse_equivalent_type[middle].id == value)
            return morse_equivalent_type[middle].value;
        else if (morse_equivalent_type[middle].id > value)
            end = middle - 1;
        else
            start = middle + 1;
    }
    return 0x00;
}

void morse_input(rt_bool_t flag)
{
    if (morse_flag != flag)
    {
        rt_tick_t interval = rt_tick_get_millisecond() - millisecond;
        millisecond = rt_tick_get_millisecond();
        if (flag)
        {
            rt_timer_stop(morse_timer);
        }
        else
        {
            if (interval <= PKG_USING_MORSE_DI_DA_SEPARATE)
                dida_add(RT_DI);
            else
            {
                dida_add(RT_DA);
            }
            morse_timer_start(PKG_USING_MORSE_INTERCHAR);
        }
        morse_flag = flag;
    }
}

int morse_init(void)
{
    rt_uint8_t var = 0;
    for (; morse_equivalent_type[var].id != 0b1; ++var);
    morse_num_max = var;
    morse_timer = rt_timer_create("morse", morse_timeout,
                                  RT_NULL, 0,
                                  RT_TIMER_FLAG_ONE_SHOT);
    return 0;
}

#ifndef PKG_USING_MORSE_SHELL
INIT_APP_EXPORT(morse_init);
#endif
