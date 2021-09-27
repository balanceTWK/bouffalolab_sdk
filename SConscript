# RT-Thread building script for component

Import('rtconfig')
Import('RTT_ROOT')
from building import *

cwd = GetCurrentDir()

src = []
CPPPATH = [cwd]
CPPDEFINES = []

src += ['bl_mcu_sdk/common/misc/misc.c']

src += ['bl_mcu_sdk/drivers/bl602_driver/startup/system_bl602.c']
src += ['bl_mcu_sdk/drivers/bl602_driver/startup/GCC/start_load.c']
src += ['bl_mcu_sdk/drivers/bl602_driver/startup/interrupt.c']

src += ['bl_mcu_sdk/drivers/bl602_driver/std_drv/src/bl602_hbn.c']
src += ['bl_mcu_sdk/drivers/bl602_driver/std_drv/src/bl602_romapi.c']
src += ['bl_mcu_sdk/drivers/bl602_driver/std_drv/src/bl602_xip_sflash_ext.c']
src += ['bl_mcu_sdk/drivers/bl602_driver/std_drv/src/bl602_sf_cfg_ext.c']
src += ['bl_mcu_sdk/drivers/bl602_driver/std_drv/src/bl602_sflash_ext.c']
src += ['bl_mcu_sdk/drivers/bl602_driver/std_drv/src/bl602_glb.c']

src += ['bl_mcu_sdk/drivers/bl602_driver/hal_drv/src/hal_flash.c']
src += ['bl_mcu_sdk/drivers/bl602_driver/hal_drv/src/hal_clock.c']
src += ['bl_mcu_sdk/drivers/bl602_driver/hal_drv/src/hal_mtimer.c']

CPPPATH += [ cwd + '/bl_mcu_sdk/common/misc ']
CPPPATH += [ cwd + '/bl_mcu_sdk/bsp/bsp_common/platform']
CPPPATH += [ cwd + '/bl_mcu_sdk/bsp/board/bl602']
CPPPATH += [ cwd + '/bl_mcu_sdk/drivers/bl602_driver/risc-v/Core/Include']
CPPPATH += [ cwd + '/bl_mcu_sdk/drivers/bl602_driver/hal_drv/inc']
CPPPATH += [ cwd + '/bl_mcu_sdk/drivers/bl602_driver/std_drv/inc']
CPPPATH += [ cwd + '/bl_mcu_sdk/drivers/bl602_driver/regs']
CPPPATH += [ cwd + '/bl_mcu_sdk/drivers/bl602_driver/startup']
CPPPATH += [ cwd + '/bl_mcu_sdk/drivers/bl602_driver/hal_drv/default_config']
CPPPATH += [ cwd + '/bl_mcu_sdk/common/ring_buffer']
CPPPATH += [ cwd + '/bl_mcu_sdk/common/soft_crc']
CPPPATH += [ cwd + '/bl_mcu_sdk/common/memheap']
CPPPATH += [ cwd + '/bl_mcu_sdk/common/list']
CPPPATH += [ cwd + '/bl_mcu_sdk/common/device']
CPPPATH += [ cwd + '/bl_mcu_sdk/common/partition']
CPPPATH += [ cwd + '/bl_mcu_sdk/common/bl_math']
CPPPATH += [ cwd + '/bl_mcu_sdk/common/pid']
CPPPATH += [ cwd + '/bl_mcu_sdk/common/timestam']

CPPDEFINES += ['ARCH_RISCV']
CPPDEFINES += ['bl602_iot']
CPPDEFINES += ['BFLB_USE_ROM_DRIVER']

if GetDepend('BSP_USING_DEBUG_UART'):
    src += ['bl_mcu_sdk/common/device/drv_device.c']
    src += ['bl_mcu_sdk/drivers/bl602_driver/std_drv/src/bl602_uart.c']

group = DefineGroup('bouffalolab_sdk', src, depend = [''], CPPPATH = CPPPATH, CPPDEFINES=CPPDEFINES)

Return('group')
