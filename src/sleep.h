#include "pico/stdlib.h"
#include "hardware/rtc.h"
#include "hardware/clocks.h"
#include "hardware/regs/m0plus.h"
#include "hardware/structs/scb.h"

void enter_sleep_mode(uint32_t sleep_seconds) {
    // Obtener la fecha y hora actual del RTC
    datetime_t current_time;
    rtc_get_datetime(&current_time);

    // Configurar el temporizador de alarma basado en la hora actual
    datetime_t alarm_time = current_time;
    alarm_time.sec += sleep_seconds;

    // Ajustar los valores de minutos y horas si es necesario
    if (alarm_time.sec >= 60) {
        alarm_time.sec -= 60;
        alarm_time.min += 1;
    }
    if (alarm_time.min >= 60) {
        alarm_time.min -= 60;
        alarm_time.hour += 1;
    }
    if (alarm_time.hour >= 24) {
        alarm_time.hour -= 24;
        alarm_time.day += 1;
        // Aquí deberías ajustar los días del mes y el mes si es necesario
    }

    // Establecer la alarma
    rtc_set_alarm(&alarm_time, NULL);

    // Poner el microcontrolador en modo de sueño
    uint32_t save = scb_hw->scr;
    scb_hw->scr = save | M0PLUS_SCR_SLEEPDEEP_BITS;
    __wfi(); // Wait for interrupt

    // Restaurar el registro SCR
    scb_hw->scr = save;
}