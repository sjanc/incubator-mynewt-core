/**
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include <assert.h>
#include <string.h>

#include "sysinit/sysinit.h"
#include "os/os.h"
#include "bsp/bsp.h"
#include "hal/hal_gpio.h"

#include "sensor/sensor.h"
#include "sensor/temperature.h"

static int g_red_led_pin = LED_1;
static int g_green_led_pin = LED_2;

static int temp_read_cb(struct sensor *sensor, void *arg, void *data, sensor_type_t type)
{
    struct sensor_temp_data *std = data;

    if (type != SENSOR_TYPE_AMBIENT_TEMPERATURE) {
        return 0;
    }

    if (!std->std_temp_is_valid) {
        hal_gpio_write(g_green_led_pin, 1);
        hal_gpio_write(g_red_led_pin, 1);
        return 0;
    }

    if (std->std_temp > 32) {
        hal_gpio_write(g_green_led_pin, 1);
        hal_gpio_write(g_red_led_pin, 0);
    } else {
        hal_gpio_write(g_red_led_pin, 1);
        hal_gpio_write(g_green_led_pin, 0);
    }

    return 0;
}

/**
 * main
 *
 * The main task for the project. This function initializes packages,
 * and then blinks the BSP LED in a loop.
 *
 * @return int NOTE: this function should never return!
 */
int
main(int argc, char **argv)
{
    static struct sensor *temp_sensor;

    sysinit();

    hal_gpio_init_out(g_red_led_pin, 1);
    hal_gpio_init_out(g_green_led_pin, 1);

    /* Look up sensor by type */
    temp_sensor = sensor_mgr_find_next_bytype(SENSOR_TYPE_AMBIENT_TEMPERATURE,
                                              NULL);
    if (!temp_sensor) {
        return -1;
    }

    while (1) {
        /* Wait one second */
        os_time_delay(OS_TICKS_PER_SEC);

        if (sensor_read(temp_sensor, SENSOR_TYPE_AMBIENT_TEMPERATURE,
                        temp_read_cb, NULL ,OS_TIMEOUT_NEVER) < 0) {
            return -1;
        }


    }

    assert(0);

    return 0;
}

