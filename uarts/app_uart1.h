/*
 * File      : app_uart.h
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006 - 2012, RT-Thread Development Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Change Logs:
 * Date           Author       Notes
 *2017-12-15      DQL          the first version 
 */

#ifndef __UART_H__
#define __UART_H__

#include <rthw.h>
#include <rtthread.h>

#define	UART_COUNT	2-1
#define UART_DELAY_TIME 2

extern uint8_t uartsDelay[UART_COUNT];
extern 	uint8_t uartsGot[UART_COUNT];

extern struct rt_serial_device serial2;

rt_err_t uart_open(const char *name);

rt_uint8_t uart_getchar(rt_device_t dev, rt_uint8_t no);

void uart_putchar(rt_device_t dev, const rt_uint8_t c);
void uart_putstring(rt_device_t dev, const rt_uint8_t *s);

int uarts_thread_init(void);

void uart2_send(rt_uint8_t no, rt_uint8_t *s);

#endif
