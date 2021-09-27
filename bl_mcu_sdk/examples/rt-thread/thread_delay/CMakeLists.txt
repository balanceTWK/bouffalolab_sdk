set(TARGET_REQUIRED_LIBS rt-thread)
set(TARGET_REQUIRED_SRCS ${CMAKE_SOURCE_DIR}/components/rt-thread/bsp/bouffalolab/libraries/drivers/board.c
${CMAKE_SOURCE_DIR}/components/rt-thread/bsp/bouffalolab/libraries/startup_gcc.S)
set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/components/rt-thread/bsp/bouffalolab/${CHIP}/${CHIP}_rtt_flash.ld)
set(mains main.c)
generate_bin()



