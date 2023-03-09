#include <rtthread.h>
#include <board.h>
#include <morse.h>
#include <shell.h>
#include <msh.h>
#include <string.h>

struct morse_shell
{
    char line[FINSH_CMD_SIZE + 1];
    rt_uint16_t line_position;
};

void morse_shell_entry(void *parameter)
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

        /* handle backspace key */
        else if (ch == 0x7f || ch == 0x08)
        {
            if (shell.line_position == 0)
                continue;

            shell.line_position--;
            rt_kprintf("\b \b");
            shell.line[shell.line_position] = 0;
            continue;
        }

        shell.line[shell.line_position] = ch;
        rt_kprintf("%c", ch);
        ch = 0;
        shell.line_position++;
    }
}

int morse_shell_example(void)
{
    rt_thread_t tid;
    tid = rt_thread_create("mshell", morse_shell_entry, RT_NULL, 4096, 20, 10);

    if (tid != NULL)
        rt_thread_startup(tid);
    return RT_EOK;
}

INIT_APP_EXPORT(morse_shell_example);
