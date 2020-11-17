/*
 * config.h
 * most of the project's configuration is done here.
 * you just have to write the tasks, and put
 * interrupt service routines in main.c.
 */

#ifndef CONFIG_H
#define CONFIG_H

/* this macro allows you to enter task time in milliseconds, regardless of
 * how often tsk_task_time_manager runs. */
#define MSEC(t) (((TaskTime)t/TASK_TIME_INTERVAL_MSEC)+1)

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include "iomacros.h"
#include "tasker.h"

// =======================
// = Include task headers
// =======================
#include "blink.h"
#include "button.h"
#include "motor.h"

// =====================
// = Input/Output
// =====================
/* Put meaningful names on I/O pins */
#define BUTTON   D,3
#define LED_DOWN D,2
#define LED_UP   D,4

// =====================
// = Other Config
// =====================
/* avr clock speed (in HZ) */
#define F_CPU 8000000UL
/* how often the tsk_task_time_manager runs (in milliseconds) */
#define TASK_TIME_INTERVAL_MSEC 4

#ifdef TASKER_C

// =====================
// = Tasks
// =====================
/* Define tasks, their initial state and their run period */
Task task_array[] = {
    { blink_upper,                PAUSED,   MSEC(4),   1 },
    { debounce,                 RUNNABLE,   MSEC(4),   1 },
    { motor_startup,              PAUSED,   MSEC(20),  1 },
    { motor_shutdown,             PAUSED,   MSEC(36),  1 },
    { motor_rampup,               PAUSED,   MSEC(36),  1 },
    { motor_rampdown,             PAUSED,   MSEC(36),  1 },
    { motor_toggle_speed_control, PAUSED,   MSEC(24),  1 },
    { button_event_handler,       PAUSED,   MSEC(4),   1 },
    { blink,                      PAUSED,   MSEC(8),   1 },
    { blink_upper,                PAUSED,   MSEC(32),  1 },
    { blink_lower,                PAUSED,   MSEC(4),   1 },
    { blink_upper_secondary,      PAUSED,   MSEC(4),   1 },
    { blink_lower_secondary,      PAUSED,   MSEC(4),   1 },
};

#endif
#ifdef MAIN_C

// =====================
// = Registers
// =====================
/* Define I/O pins as inputs or outputs, and other I/O settings */
static void io_init(void)
{
    INPUT(BUTTON);
    PULLUP(BUTTON);

    OUTPUT(LED_UP);
    OFF(LED_UP);

    OUTPUT(LED_DOWN);
    ON(LED_DOWN);
}
/* This is the timer-counter's initial value.
   The timer-counter will be initialiez to this value
   on startup (in here) and in timer's overflow interrupt routine. */
#define TCNT0_VALUE 0x83
static void timer0_init(void)
{
    TCCR0 =
        (1 << CS02) |
        (0 << CS01) |
        (0 << CS00);

    TCNT0 = TCNT0_VALUE;
}
static void timer1_init(void)
{
    TCCR1A =
        (1 << COM1A1) |
        (1 << COM1A0) |
        (0 << COM1B1) |
        (0 << COM1B0) |
        (0 << FOC1A)  |
        (1 << FOC1B)  |
        (0 << WGM11)  |
        (0 << WGM10);

    TCCR1B =
        (0 << ICNC1) |
        (0 << ICES1) |
        (1 << WGM13) |
        (0 << WGM12) |
        (0 << CS12)  |
        (0 << CS11)  |
        (1 << CS10);

    TCNT1 = 0;
    ICR1  = 100; /* TOP = 100; */
    OCR1A = 0;
    OCR1B = 0;
}
static void timer_interrupts_init(void)
{
    TIMSK =
        (0 << OCIE2)  |
        (0 << TOIE2)  |
        (0 << TICIE1) |
        (0 << OCIE1A) |
        (0 << OCIE1B) |
        (0 << TOIE1)  |
        (1 << TOIE0);
}
static void registers_init(void)
{
    io_init();
    timer0_init();
    timer1_init();
    timer_interrupts_init();
}
ISR(TIMER0_OVF_vect)
{
    TCNT0 = TCNT0_VALUE;
    tsk_task_time_manager();
}

#endif /* MAIN_C */
#endif /* CONFIG_H */