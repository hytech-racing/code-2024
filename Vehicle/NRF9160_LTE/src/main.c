/*
 * Copyright (c) 2018 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(mqtt_simple, CONFIG_MQTT_SIMPLE_LOG_LEVEL);
//#include "uart.c"
#include "mqtt.h"


#define THREAD_PRIORITY 7

K_THREAD_DEFINE(mqtt_threadid, CONFIG_MAIN_STACK_SIZE, mqtt_thread, NULL, NULL, NULL, THREAD_PRIORITY, 0, 0);
K_THREAD_DEFINE(uart_threadid, CONFIG_MAIN_STACK_SIZE, uart_thread, NULL, NULL, NULL, THREAD_PRIORITY, 0, 0);