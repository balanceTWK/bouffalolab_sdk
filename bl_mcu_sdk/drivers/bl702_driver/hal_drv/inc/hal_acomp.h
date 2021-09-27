#ifndef __HAL_ACOMP_H__
#define __HAL_ACOMP_H__

#include "drv_device.h"
#include "bl702_config.h"

enum acomp_channel_type {
    ACOMP_CHANNEL_ADC_CHANNEL0, /*!< Analog compare channel,ADC input channel 0 */
    ACOMP_CHANNEL_ADC_CHANNEL1, /*!< Analog compare channel,ADC input channel 1 */
    ACOMP_CHANNEL_ADC_CHANNEL2, /*!< Analog compare channel,ADC input channel 2 */
    ACOMP_CHANNEL_ADC_CHANNEL3, /*!< Analog compare channel,ADC input channel 3 */
    ACOMP_CHANNEL_ADC_CHANNEL4, /*!< Analog compare channel,ADC input channel 4 */
    ACOMP_CHANNEL_ADC_CHANNEL5, /*!< Analog compare channel,ADC input channel 5 */
    ACOMP_CHANNEL_ADC_CHANNEL6, /*!< Analog compare channel,ADC input channel 6 */
    ACOMP_CHANNEL_ADC_CHANNEL7, /*!< Analog compare channel,ADC input channel 7 */
    ACOMP_CHANNEL_DAC_CHANNELA, /*!< Analog compare channel,DAC output channel A */
    ACOMP_CHANNEL_DAC_CHANNELB, /*!< Analog compare channel,DAC output channel B */
    ACOMP_CHANNEL_VREF_1P2V,    /*!< Analog compare channel,1.2V ref voltage */
    ACOMP_CHANNEL_0P375VBAT,    /*!< Analog compare channel,6/16Vbat */
    ACOMP_CHANNEL_0P25VBAT,     /*!< Analog compare channel,4/16Vbat */
    ACOMP_CHANNEL_0P1875VBAT,   /*!< Analog compare channel,3/16Vbat */
    ACOMP_CHANNEL_0P3125VBAT,   /*!< Analog compare channel,5/16Vbat */
    ACOMP_CHANNEL_VSS,          /*!< Analog compare channel,vss */
};

enum acomp_hysteresis_vol_type {
    ACOMP_HYSTERESIS_VOLT_NONE, /*!< Analog compare hysteresis voltage none */
    ACOMP_HYSTERESIS_VOLT_10MV, /*!< Analog compare hysteresis voltage 10mv */
    ACOMP_HYSTERESIS_VOLT_20MV, /*!< Analog compare hysteresis voltage 20mv */
    ACOMP_HYSTERESIS_VOLT_30MV, /*!< Analog compare hysteresis voltage 30mv */
    ACOMP_HYSTERESIS_VOLT_40MV, /*!< Analog compare hysteresis voltage 40mv */
    ACOMP_HYSTERESIS_VOLT_50MV, /*!< Analog compare hysteresis voltage 50mv */
    ACOMP_HYSTERESIS_VOLT_60MV, /*!< Analog compare hysteresis voltage 60mv */
    ACOMP_HYSTERESIS_VOLT_70MV, /*!< Analog compare hysteresis voltage 70mv */
};

typedef struct acomp_device {
    struct device parent;
    uint8_t id;
    enum acomp_channel_type pos_ch;
    enum acomp_channel_type neg_ch;
    enum acomp_hysteresis_vol_type pos_hysteresis_vol;
    enum acomp_hysteresis_vol_type neg_hysteresis_vol;
} acomp_device_t;

void acomp_init(acomp_device_t *device);
int acomp_get_result(acomp_device_t *device);
#endif