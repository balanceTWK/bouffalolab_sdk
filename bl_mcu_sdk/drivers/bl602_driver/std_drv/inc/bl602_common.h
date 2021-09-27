#ifndef __BL602_COMMON_H__
#define __BL602_COMMON_H__

#include "bl602.h"
#include "bflb_platform.h"

#ifndef __NOP
#define __NOP() __ASM volatile("nop") /* This implementation generates debug information */
#endif
#ifndef __WFI
#define __WFI() __ASM volatile("wfi") /* This implementation generates debug information */
#endif
#ifndef __WFE
#define __WFE() __ASM volatile("wfe") /* This implementation generates debug information */
#endif
#ifndef __SEV
#define __SEV() __ASM volatile("sev") /* This implementation generates debug information */
#endif

__attribute__((always_inline)) __STATIC_INLINE uint32_t __REV(uint32_t value)
{
    //return __builtin_bswap32(value);
    uint32_t res = 0;

    res = (value << 24) | (value >> 24);
    res &= 0xFF0000FF; /* only for sure */
    res |= ((value >> 8) & 0x0000FF00) | ((value << 8) & 0x00FF0000);

    return res;
}

__attribute__((always_inline)) __STATIC_INLINE uint32_t __REV16(uint32_t value)
{
    return __builtin_bswap16(value);
}

#define arch_delay_us BL602_Delay_US
#define arch_delay_ms BL602_Delay_MS

void BL602_Delay_US(uint32_t cnt);
void BL602_Delay_MS(uint32_t cnt);

#endif
