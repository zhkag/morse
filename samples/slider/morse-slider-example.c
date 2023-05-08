/*
 * Copyright (c) 2006-2023, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-07-28     Rbb666       first version
 */

#include <rtthread.h>
#include "drv_common.h"
#include <morse.h>

#ifdef BSP_USING_SLIDER
#include "cycfg_capsense.h"

#define CAPSENSE_INTR_PRIORITY      (7u)
#define EZI2C_INTR_PRIORITY         (6u)

/* Allowed duty cycle for maximum brightness */
#define LED_MAX_BRIGHTNESS      (100u)

/* Allowed duty cycle for minimum brightness*/
#define LED_MIN_BRIGHTNESS      (0u)

#define GET_DUTY_CYCLE(x)       (1 * 1000 * 1000 - x * 10 * 1000)

static rt_sem_t trans_done_semphr = RT_NULL;
static rt_thread_t sld_thread = RT_NULL;

static void capsense_isr(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    Cy_CapSense_InterruptHandler(CYBSP_CSD_HW, &cy_capsense_context);

    /* leave interrupt */
    rt_interrupt_leave();
}

void capsense_callback(cy_stc_active_scan_sns_t *ptrActiveScan)
{
    rt_sem_release(trans_done_semphr);
}

static uint32_t initialize_capsense(void)
{
    uint32_t status = CYRET_SUCCESS;

    /* CapSense interrupt configuration parameters */
    static const cy_stc_sysint_t capSense_intr_config =
    {
        .intrSrc = csd_interrupt_IRQn,
        .intrPriority = CAPSENSE_INTR_PRIORITY,
    };

    /* Capture the CSD HW block and initialize it to the default state. */
    status = Cy_CapSense_Init(&cy_capsense_context);
    if (CYRET_SUCCESS != status)
    {
        return status;
    }

    /* Initialize CapSense interrupt */
    cyhal_system_set_isr(csd_interrupt_IRQn, csd_interrupt_IRQn, CAPSENSE_INTR_PRIORITY, &capsense_isr);
    NVIC_ClearPendingIRQ(capSense_intr_config.intrSrc);
    NVIC_EnableIRQ(capSense_intr_config.intrSrc);

    /* Initialize the CapSense firmware modules. */
    status = Cy_CapSense_Enable(&cy_capsense_context);
    if (CYRET_SUCCESS != status)
    {
        return status;
    }

    /* Assign a callback function to indicate end of CapSense scan. */
    status = Cy_CapSense_RegisterCallback(CY_CAPSENSE_END_OF_SCAN_E,
                                          capsense_callback, &cy_capsense_context);
    if (CYRET_SUCCESS != status)
    {
        return status;
    }

    return status;
}

void Slider_Init(void)
{
    cy_rslt_t result;

    result = initialize_capsense();

    if (CYRET_SUCCESS != result)
    {
        /* Halt the CPU if CapSense initialization failed */
        RT_ASSERT(0);
    }

    /* Initiate first scan */
    Cy_CapSense_ScanAllWidgets(&cy_capsense_context);

    trans_done_semphr = rt_sem_create("slider_sem", 1, RT_IPC_FLAG_PRIO);
    if (trans_done_semphr == RT_NULL)
    {
        rt_kprintf("create transform done semphr failed.\n");
        RT_ASSERT(0);
        return;
    }

}

static void process_touch(void)
{
    cy_stc_capsense_touch_t *slider_touch_info;
    uint16_t slider_pos;
    uint8_t slider_touch_status;

    /* Get slider status */
    slider_touch_info = Cy_CapSense_GetTouchInfo(
                            CY_CAPSENSE_LINEARSLIDER0_WDGT_ID, &cy_capsense_context);
    slider_touch_status = slider_touch_info->numPosition;

    morse_input(slider_touch_status);
}

static void Slider_thread_entry(void *parameter)
{
    Slider_Init();

    for (;;)
    {
        rt_sem_take(trans_done_semphr, RT_WAITING_FOREVER);

        /* Process all widgets */
        Cy_CapSense_ProcessAllWidgets(&cy_capsense_context);

        /* Process touch input */
        process_touch();

        /* Establishes synchronized operation between the CapSense
         * middleware and the CapSense Tuner tool.
         */
        Cy_CapSense_RunTuner(&cy_capsense_context);

        /* Initiate next scan */
        Cy_CapSense_ScanAllWidgets(&cy_capsense_context);

        rt_thread_mdelay(10);
    }
}

int Slider_ctrl_sample(void)
{
    rt_err_t ret = RT_EOK;

    sld_thread = rt_thread_create("slider_th",
                                  Slider_thread_entry,
                                  RT_NULL,
                                  1024,
                                  25,
                                  10);
    if (sld_thread != RT_NULL)
    {
        rt_thread_startup(sld_thread);
    }
    else
    {
        ret = -RT_ERROR;
    }

    return ret;
}
INIT_APP_EXPORT(Slider_ctrl_sample);
#endif
