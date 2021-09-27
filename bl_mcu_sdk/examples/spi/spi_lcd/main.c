/**
 * @file main.c
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
#include "hal_spi.h"
#include "hal_gpio.h"
#include "hal_dma.h"
#include "bsp_il9341.h"

uint8_t bo[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x06, 0x00,
    0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x04, 0x00, 0x30, 0x00, 0x04, 0x0C, 0xFC, 0xFF, 0xFF, 0x0F,
    0x80, 0xFF, 0xFF, 0x07, 0x00, 0x00, 0x08, 0x00, 0x00, 0x08, 0x08, 0x00, 0x00, 0x08, 0x08, 0x00,
    0x00, 0x08, 0x00, 0x00, 0x00, 0x08, 0x30, 0x00, 0x00, 0xC8, 0x3F, 0x00, 0x00, 0x08, 0x20, 0x00,
    0x00, 0x0B, 0x29, 0x01, 0x80, 0x1B, 0x29, 0x03, 0x80, 0x39, 0xFF, 0x3F, 0x00, 0xD0, 0xFF, 0x3F,
    0x08, 0x10, 0x5A, 0x06, 0x0C, 0x10, 0x52, 0x06, 0x3E, 0xF0, 0x92, 0x04, 0xFC, 0x7F, 0x80, 0x04,
    0x00, 0xB0, 0x8F, 0x10, 0x00, 0x30, 0xFF, 0x18, 0x00, 0x30, 0x60, 0x0C, 0x00, 0x20, 0x00, 0x0C,
    0x00, 0x20, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

uint8_t liu[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x0C, 0x00,
    0xE0, 0x00, 0x06, 0x00, 0xF0, 0x03, 0x07, 0x00, 0x80, 0x0F, 0x00, 0x03, 0x00, 0x38, 0x80, 0x03,
    0x08, 0x40, 0x80, 0x01, 0x08, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x00,
    0xC0, 0xDF, 0x20, 0x00, 0x00, 0xCF, 0x21, 0x00, 0x00, 0x88, 0x63, 0x00, 0x00, 0x80, 0x67, 0x00,
    0x20, 0x98, 0x7D, 0x00, 0xF0, 0x1F, 0x79, 0x18, 0x00, 0x00, 0x51, 0x0C, 0x00, 0x00, 0xC1, 0x0E,
    0x00, 0x40, 0xCA, 0x00, 0xE0, 0x3F, 0xCE, 0x00, 0x70, 0x30, 0x8E, 0x00, 0x30, 0x00, 0x87, 0x01,
    0x18, 0x00, 0x83, 0x00, 0x18, 0x00, 0x80, 0x00, 0x18, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00,
    0x30, 0x00, 0x00, 0x00, 0xF0, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

uint8_t zhi[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00,
    0x00, 0x00, 0x06, 0x00, 0x00, 0x08, 0x06, 0x00, 0x00, 0x08, 0x04, 0x00, 0x00, 0x10, 0x64, 0x00,
    0x00, 0x20, 0xC4, 0x00, 0x00, 0xC0, 0x88, 0x03, 0x00, 0x80, 0x8B, 0x3E, 0x00, 0x30, 0xFF, 0x1C,
    0xFC, 0x1F, 0xF8, 0x11, 0x10, 0x11, 0x12, 0x01, 0x10, 0x11, 0x13, 0x01, 0x10, 0xA2, 0x13, 0x03,
    0x10, 0xA2, 0x11, 0x02, 0x10, 0x22, 0x00, 0x00, 0x30, 0x22, 0x80, 0x00, 0x10, 0x60, 0xFC, 0x00,
    0x1E, 0x70, 0x4E, 0x00, 0xFE, 0x7F, 0x82, 0x00, 0xF0, 0x3F, 0x84, 0x00, 0x00, 0x10, 0x84, 0x00,
    0x00, 0x00, 0x84, 0x00, 0x00, 0x00, 0x8C, 0x01, 0x00, 0x00, 0xFC, 0x01, 0x00, 0x00, 0xE0, 0x00,
    0x00, 0x00, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

uint8_t neng[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x38, 0x00, 0x0C, 0x00, 0xF0, 0x01, 0x1C, 0x00, 0x80, 0xFF, 0x6D, 0x00, 0x00, 0x12, 0xC9, 0x00,
    0x00, 0x12, 0x89, 0x07, 0x20, 0x32, 0x09, 0x1E, 0x30, 0x10, 0x11, 0x0C, 0xFC, 0xFF, 0x93, 0x00,
    0xF8, 0xFF, 0xF1, 0x00, 0x00, 0x00, 0x78, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x80, 0x00, 0x18, 0xE0, 0xFF, 0xFC, 0x1F, 0x30, 0x44, 0x46, 0x08, 0x18, 0x0C, 0xC6, 0x00,
    0x18, 0x08, 0x82, 0x00, 0x18, 0x18, 0x82, 0x01, 0x18, 0x70, 0x02, 0x03, 0x18, 0x70, 0x06, 0x03,
    0x18, 0x00, 0x06, 0x00, 0x18, 0x00, 0x3E, 0x00, 0x38, 0x00, 0x00, 0x00, 0xF0, 0x01, 0x00, 0x00,
    0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

uint8_t ke[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x00, 0x00, 0x02, 0x03, 0x00,
    0x00, 0x04, 0x03, 0x00, 0x00, 0x08, 0x02, 0x00, 0x00, 0x10, 0x46, 0x00, 0x00, 0x60, 0x46, 0x00,
    0x00, 0xC0, 0x84, 0x00, 0x60, 0x80, 0x87, 0x01, 0xF8, 0xFF, 0xFF, 0x03, 0x00, 0x80, 0x68, 0x02,
    0x00, 0xC0, 0x08, 0x0E, 0x00, 0x40, 0x18, 0x0C, 0x00, 0x20, 0x18, 0x04, 0x00, 0x20, 0x00, 0x00,
    0x00, 0x20, 0x00, 0x00, 0x00, 0x20, 0x04, 0x00, 0x00, 0x20, 0xC6, 0x00, 0x00, 0x60, 0xC6, 0x00,
    0x00, 0x40, 0x60, 0x00, 0x00, 0x40, 0x00, 0x10, 0xE0, 0xFF, 0xFF, 0x1F, 0xFC, 0xFF, 0xFF, 0x1F,
    0x00, 0xC0, 0x00, 0x08, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00,
    0x00, 0x80, 0x01, 0x00, 0x00, 0x80, 0x01, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

uint8_t ji[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x0E, 0x00, 0x00,
    0x00, 0x0E, 0x08, 0x00, 0x00, 0x0C, 0x08, 0x00, 0x00, 0x18, 0x08, 0x00, 0x20, 0x30, 0x08, 0x00,
    0x30, 0x20, 0x18, 0x00, 0xF8, 0xFF, 0xFF, 0x1F, 0xF0, 0xFF, 0xFF, 0x1F, 0x00, 0x80, 0x10, 0x08,
    0x00, 0x00, 0x11, 0x00, 0x10, 0x00, 0x00, 0x00, 0x20, 0x40, 0x00, 0x00, 0x20, 0x50, 0x10, 0x00,
    0x60, 0x50, 0x10, 0x00, 0x60, 0xC8, 0x30, 0x00, 0xC0, 0x8C, 0x30, 0x00, 0x80, 0x87, 0x3F, 0x1C,
    0x80, 0x83, 0xFF, 0x1F, 0x80, 0x8F, 0x61, 0x0E, 0xC0, 0xFC, 0x61, 0x00, 0xE0, 0xE0, 0x61, 0x00,
    0x70, 0x80, 0x40, 0x00, 0x30, 0x00, 0x40, 0x00, 0x30, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00,
    0x10, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
void LCD_Display_Test(void)
{
    LCD_Clear(RGB(0, 0, 0));
    LCD_DrawLine(200, 200, 50, 50, RGB(255, 0, 255));
    LCD_DrawRectangle(40, 70, 150, 300, RGB(0, 255, 0));
    LCD_DrawCircle(100, 100, 60, RGB(0, 0, 255));
    LCD_Clear(RGB(0, 0, 0));
    //    LCD_DrawPicture(130,110,185,209,image);
    LCD_DrawChinese(60, 20, bo, RGB(0, 0, 0), RGB(0, 255, 0));
    LCD_DrawChinese(60, 70, liu, RGB(0, 0, 0), RGB(0, 255, 0));
    LCD_DrawChinese(60, 120, zhi, RGB(0, 0, 0), RGB(0, 255, 0));
    LCD_DrawChinese(60, 170, neng, RGB(0, 0, 0), RGB(0, 255, 0));
    LCD_DrawChinese(60, 220, ke, RGB(0, 0, 0), RGB(0, 255, 0));
    LCD_DrawChinese(60, 270, ji, RGB(0, 0, 0), RGB(0, 255, 0));
}

int main(void)
{
    bflb_platform_init(0);

    LCD_Init();

    LCD_Display_Test();

    BL_CASE_SUCCESS;
    while (1) {
        bflb_platform_delay_ms(100);
    }
}
