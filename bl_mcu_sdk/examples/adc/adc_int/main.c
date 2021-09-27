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
#include "hal_adc.h"
#include "hal_gpio.h"

adc_channel_t posChList[] = { ADC_CHANNEL3 };
adc_channel_t negChList[] = { ADC_CHANNEL_GND };

void adc_irq_callback(struct device *dev, void *args, uint32_t size, uint32_t state)
{
    if (state == ADC_EVENT_FIFO) {
        MSG("FIFO Ready Interrupt Happend \r\n");
        adc_channel_val_t *adc_parse_val = (adc_channel_val_t *)args;
        for (uint32_t i = 0; i < size; i++) {
            MSG("PosId = %d NegId = %d V= %d mV ", adc_parse_val[i].posChan, adc_parse_val[i].negChan, (uint32_t)(adc_parse_val[i].volt * 1000));
        }
        BL_CASE_SUCCESS;
        device_control(dev, DEVICE_CTRL_CLR_INT, (void *)(ADC_FIFO_IT));
    } else {
    }
}

struct device *adc_test;

int main(void)
{
    bflb_platform_init(0);

    adc_channel_cfg_t adc_channel_cfg;

    adc_channel_cfg.pos_channel = posChList;
    adc_channel_cfg.neg_channel = negChList;
    adc_channel_cfg.num = 1;

    adc_register(ADC0_INDEX, "adc");

    adc_test = device_find("adc");

    if (adc_test) {
        device_open(adc_test, DEVICE_OFLAG_INT_RX);
        device_set_callback(adc_test, adc_irq_callback);
        device_control(adc_test, DEVICE_CTRL_SET_INT, (void *)(ADC_FIFO_IT));

        if (device_control(adc_test, DEVICE_CTRL_ADC_CHANNEL_CONFIG, &adc_channel_cfg) == ERROR) {
            MSG("ADC channel config error , Please check the channel corresponding to IO is initial success by board system or Channel is invaild \r\n");
            BL_CASE_FAIL;
            while (1)
                ;
        }
        MSG("adc device find success\r\n");
    }

    adc_channel_start(adc_test);

    while (1)
        bflb_platform_delay_ms(100);
}