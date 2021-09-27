/**
 * @file st7789v.h
 * @brief
 *
 * Copyright (c) 2021 Bouffalolab team
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 */

#ifndef _ST7789V_H_
#define _ST7789V_H_

#include "bflb_platform.h"

#define ST7789V_CS_PIN GPIO_PIN_10
#define ST7789V_DC_PIN GPIO_PIN_22

#define ST7789V_CS_HIGH gpio_write(ST7789V_CS_PIN, 1)
#define ST7789V_CS_LOW  gpio_write(ST7789V_CS_PIN, 0)
#define ST7789V_DC_HIGH gpio_write(ST7789V_DC_PIN, 1)
#define ST7789V_DC_LOW  gpio_write(ST7789V_DC_PIN, 0)

#define ST7789V_W 135 /* ST7789V LCD width */
#define ST7789V_H 240 /* ST7789V LCD height */

#define ST7789V_OFFSET_X 53
#define ST7789V_OFFSET_Y 40

typedef struct {
    uint8_t cmd;
    const char *data;
    uint8_t databytes; /* Num of data in data; bit 7 = delay after set */
} st7789v_init_cmd_t;

int st7789v_init();
void st7789v_set_dir(uint8_t dir);
void st7789v_set_draw_window(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2);
void st7789v_draw_point(uint16_t x, uint16_t y, uint16_t color);
void st7789v_draw_area(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void st7789v_draw_picture_nonblocking(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t *picture);
void st7789v_draw_picture_blocking(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t *picture);
int st7789v_draw_is_busy(void);

#endif