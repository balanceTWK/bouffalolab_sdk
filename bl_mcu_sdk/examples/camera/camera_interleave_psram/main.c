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

#include "bflb_platform.h"
#include "bsp_sf_psram.h"
#include "bsp_image_sensor.h"
#include "bl702_uart.h"

#define CAMERA_RESOLUTION_X (640)
#define CAMERA_RESOLUTION_Y (480)
#define CAMERA_FRAME_SIZE   (CAMERA_RESOLUTION_X * CAMERA_RESOLUTION_Y * 2)
#define CAMERA_WRITE_ADDR   (0x26000000)
#define CAMERA_BUFFER_SIZE  (0x500000)

#define YUV422_FRAME_SIZE (CAMERA_RESOLUTION_X * CAMERA_RESOLUTION_Y * 2)
#define YUV420_FRAME_SIZE (640 * 480 * 2 * 3 / 4)
#define YUV400_FRAME_SIZE (640 * 480)
#define YUV_USE           (1)

static cam_device_t camera_cfg = {
    .software_mode = CAM_MANUAL_MODE,
    .frame_mode = CAM_FRAME_INTERLEAVE_MODE,
    .yuv_format = CAM_YUV_FORMAT_YUV422,
    .cam_write_ram_addr = CAMERA_WRITE_ADDR,
    .cam_write_ram_size = CAMERA_BUFFER_SIZE,
    .cam_frame_size = CAMERA_FRAME_SIZE,

    .cam_write_ram_addr1 = 0,
    .cam_write_ram_size1 = 0,
    .cam_frame_size1 = 0,
};

static mjpeg_device_t mjpeg_cfg;

int main(void)
{
    uint8_t psramId[8] = { 0 };
    uint32_t i;
    uint8_t *picture;
    uint32_t length;

    bflb_platform_init(0);

    MSG("camera case in interleave mode in psram\n");

    bsp_sf_psram_init(1);
    bsp_sf_psram_read_id(psramId);
    MSG("PSRAM ID: %02X %02X %02X %02X %02X %02X %02X %02X.\r\n",
        psramId[0], psramId[1], psramId[2], psramId[3], psramId[4], psramId[5], psramId[6], psramId[7]);

    cam_clk_out();

    cam_hsync_crop(0, 2 * CAMERA_RESOLUTION_X);
    cam_vsync_crop(0, CAMERA_RESOLUTION_Y);

    if (SUCCESS != image_sensor_init(DISABLE, &camera_cfg, &mjpeg_cfg)) {
        MSG("Init error!\n");
        BL_CASE_FAIL;
        while (1) {
        }
    }

    cam_start();

    for (i = 0; i < 5; i++) {
        while (SUCCESS != cam_get_one_frame_interleave(&picture, &length)) {
        }

        MSG("picturestartuyvy");
        UART_SendData(0, picture, length);
        MSG("pictureend");

        cam_drop_one_frame_interleave();
    }

    cam_stop();

    BL_CASE_SUCCESS;
    while (1) {
        bflb_platform_delay_ms(100);
    }
}
