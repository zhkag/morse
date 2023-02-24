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

struct morse_shell
{
    char line[FINSH_CMD_SIZE + 1];
    rt_uint16_t line_position;
};

void morse_thread_entry(void *parameter)
{
    int ch;
    struct morse_shell shell;
    memset(shell.line, 0, sizeof(shell.line));
    while (1)
    {
        ch = morse_getc();
        if (ch < 0)
        {
            continue;
        }

        if (ch == '\0' || ch == 0xFF)
            continue;

        if (ch == '\r' || ch == '\n')
        {
            rt_kprintf("\n");
            msh_exec(shell.line, shell.line_position);
            rt_kprintf(FINSH_PROMPT);
            memset(shell.line, 0, sizeof(shell.line));
            shell.line_position = 0;
            continue;
        }

        shell.line[shell.line_position] = ch;
        rt_kprintf("%c", ch);
        ch = 0;
        shell.line_position++;
    }
}

int morse_key_example(void)
{
    rt_thread_t tid;
    rt_pin_mode(USER_KEY, PIN_MODE_INPUT_PULLUP);
    rt_pin_attach_irq(USER_KEY, PIN_IRQ_MODE_RISING_FALLING, irq_callback, RT_NULL);
    rt_pin_irq_enable(USER_KEY, PIN_IRQ_ENABLE);
    morse_init();
    tid = rt_thread_create("mshell", morse_thread_entry, RT_NULL, 4096, 20, 10);

    if (tid != NULL)
        rt_thread_startup(tid);
}

INIT_APP_EXPORT(morse_key_example);
