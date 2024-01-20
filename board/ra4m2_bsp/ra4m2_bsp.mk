BSP_SRCS=  \
		sdk/ra/fsp/src/bsp/cmsis/Device/RENESAS/Source/startup.c \
		sdk/ra/fsp/src/bsp/cmsis/Device/RENESAS/Source/system.c \
		sdk/ra/fsp/src/bsp/mcu/all/bsp_clocks.c \
		sdk/ra/fsp/src/bsp/mcu/all/bsp_common.c \
		sdk/ra/fsp/src/bsp/mcu/all/bsp_delay.c \
		sdk/ra/fsp/src/bsp/mcu/all/bsp_group_irq.c \
		sdk/ra/fsp/src/bsp/mcu/all/bsp_guard.c \
		sdk/ra/fsp/src/bsp/mcu/all/bsp_io.c \
		sdk/ra/fsp/src/bsp/mcu/all/bsp_irq.c \
		sdk/ra/fsp/src/bsp/mcu/all/bsp_register_protection.c \
		sdk/ra/fsp/src/bsp/mcu/all/bsp_rom_registers.c \
		sdk/ra/fsp/src/bsp/mcu/all/bsp_sbrk.c \
		sdk/ra/fsp/src/bsp/mcu/all/bsp_security.c \
		sdk/ra/fsp/src/r_adc/r_adc.c \
		sdk/ra/fsp/src/r_can/r_can.c \
		sdk/ra/fsp/src/r_agt/r_agt.c \
		sdk/ra/fsp/src/r_gpt/r_gpt.c \
		sdk/ra/fsp/src/r_ioport/r_ioport.c \
		sdk/ra/fsp/src/r_sci_i2c/r_sci_i2c.c \
		sdk/ra/fsp/src/r_sci_uart/r_sci_uart.c \
		sdk/ra/fsp/src/r_icu/r_icu.c \
		sdk/ra/fsp/src/r_flash_hp/r_flash_hp.c \
		sdk/ra_gen/common_data.c \
		sdk/ra_gen/hal_data.c \
		sdk/ra_gen/pin_data.c \
		sdk/ra_gen/vector_data.c \
		
#		sdk/src/hal_entry.c \
		


BSP_SRCS+= timer_hw/timer_hw.c \
		adc_hw/adc_hw.c \
		core_hw/core_hw.c \
		can_hardware/can_hardware.c \
		io_hw/io_hw.c \
		flash/flash.c \
		uart_hw/uart_hw.c \
		flash_hw/flash_hw.c
		
# CFLAGS
#######################################
# cpu
CPU = -mcpu=cortex-m33

# fpu
#FPU = -mfpu=fpv5-sp-d16

# float-abi	

# mcu
CFLAGS+= $(CPU) -mthumb $(FPU) $(FLOAT-ABI)
ASFLAGS+= $(CPU) -mthumb $(FPU) $(FLOAT-ABI)	
		
# macros for gcc
# AS defines
AS_DEFS = 

# C defines
C_DEFS = -D_RENESAS_RA_ 
		
BSP_INCLUDES = . timer_hw \
		core_hw \
		adc_hw \
		can_hardware \
		io_hw \
		flash \
		uart_hw \
		flash_hw \
		sdk/ra/fsp/src/bsp/cmsis/Device/RENESAS/Include \
		sdk/ra/fsp/src/bsp/mcu/all \
		sdk/ra/fsp/src/bsp/mcu/ra4m2 \
		sdk/ra/arm/CMSIS_5/CMSIS/Core/Include \
		sdk/ra/fsp/inc \
		sdk/ra/fsp/inc/api \
		sdk/ra/fsp/inc/instances \
		sdk/ra_cfg/fsp_cfg \
		sdk/ra_cfg/fsp_cfg/bsp \
		sdk/ra_gen \
		
ASFLAGS+= $(AS_DEFS) $(AS_INCLUDES)

CFLAGS+= $(C_DEFS) $(C_INCLUDES)
					
#######################################
# LDFLAGS
#######################################
# link script
LD_FILES:=fsp.ld