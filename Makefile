# Prefix for the arm-eabi-none toolchain.
# I'm using codesourcery g++ lite compilers available here:
# http://www.mentor.com/embedded-software/sourcery-tools/sourcery-codebench/editions/lite-edition/
# Microcontroller properties.

export TARGET=bpa
export PROJ_ROOT=.
export BOARD_TYPE=ra4m2_bsp
include $(PROJ_ROOT)/board/$(BOARD_TYPE)/$(BOARD_TYPE).mk
include $(PROJ_ROOT)/gcc.mk

SRCS:=board/$(BOARD_TYPE)/board.c
SRCS+= app/app_co/app_co.c \
	app/app_bat_port/app_bat_port.c \
	app/app_io/app_io.c \
	app/bs_app.c \
	app/app_co/app_co_init/app_co_init.c \
	app/app_co/app_co_od/app_co_od.c \
	app/app_co/app_co_od/app_co_comm_od.c \
	app/app_co/app_co_od/app_co_manu_od.c \
	app/app_co/app_co_storage_init/app_co_storage_init.c \

SRCS+= component/switch/switch.c \
	component/bat_port/bat_port.c \
	component/io_control/io_control.c \
	component/bp_data/bp_data.c \
	component/adc_sensor/adc_sensor.c \
	
SRCS+= service/can_master/can_master.c \
	  service/crc/CRC.c \
	  
SRCS+= libs/selex-libc/canopen_clib/CO_CAN_Msg.c \
	  libs/selex-libc/canopen_clib/CO_NMT.c \
	  libs/selex-libc/canopen_clib/CO_Object.c \
	  libs/selex-libc/canopen_clib/CO_OD.c \
	  libs/selex-libc/canopen_clib/CO_PDO.c \
	  libs/selex-libc/canopen_clib/CO_RPDO.c \
	  libs/selex-libc/canopen_clib/CO_SDO.c \
	  libs/selex-libc/canopen_clib/CO_SDOclient.c \
	  libs/selex-libc/canopen_clib/CO_SDOserver.c \
	  libs/selex-libc/canopen_clib/CO_SYNC.c \
	  libs/selex-libc/canopen_clib/CO_TPDO.c \
	  libs/selex-libc/canopen_clib/CO.c \
	  libs/selex-libc/canopen_clib/CO_EMCY.c \
	  	  
BSP_SRCS:=$(addprefix board/$(BOARD_TYPE)/,$(BSP_SRCS))
BSP_INCLUDES:=$(addprefix board/$(BOARD_TYPE)/,$(BSP_INCLUDES))

#INCLUDES:=. app_config board service component util app
INCLUDES+= app \
	app/app_co \
	app/app_bat_port \
	app/app_io \
	app/app_co/app_co_init \
	app/app_co/app_co_od \
	app/app_co/app_co_storage_init 
	
INCLUDES+= app_config
INCLUDES+= board/$(BOARD_TYPE)
INCLUDES+=component/switch \
	component/button \
	component/bat_port \
	component/bp_data \
	component/io_control \
	component/estimate \
	component/adc_sensor \
	component/charger \
	component/delay \
	component/uart \
	component/au_bat \
	component/buck_converter \
	component/ev_data \
	
INCLUDES+= service/can_master \
          service/string/string_util \
          service/energy \
          service/app_update \
          service/crc \
          service/ev_lock_modes \
          libs/selex-libc/canopen_clib \
          

#USER_LIB_INCLUDES=$(PROJ_ROOT)/libs/selex-libc/canopen_clib 
#USER_LIBS=CANopen
#
#LIB_INCLUDES:=libs
#LIB_INCLUDES+=
OBJDIR=build

INCLUDES+=$(LIB_INCLUDES)			
INCLUDES+=$(BSP_INCLUDES)
INCLUDES:=$(addprefix -I$(PROJ_ROOT)/,$(INCLUDES))
INCLUDES+=$(addprefix -I,$(USER_LIB_INCLUDES))

SRCS+=$(BSP_SRCS)
	
OBJS:=$(addprefix $(PROJ_ROOT)/$(OBJDIR)/,$(SRCS))		
SRCS:=$(addprefix $(PROJ_ROOT)/,$(SRCS))

SRCS+=$(PROJ_ROOT)/main.c
OBJS+=$(PROJ_ROOT)/$(OBJDIR)/main.c


OBJS:= $(patsubst %.c,%.o,$(OBJS))
OBJS:= $(patsubst %.s,%.o,$(OBJS))
DEPS:= $(patsubst %.o,%.d,$(OBJS))

LDSCRIPT_INC=
DEFS:=

#--------------------------------------------------------

OPTIMIZE=-O0
#OPTIMIZE=-O3

# Option arguments for C compiler.
CFLAGS+= $(INCLUDES)
CFLAGS+= $(OPTIMIZE)
CFLAGS+=-fmessage-length=0
CFLAGS+=-fsigned-char
CFLAGS+=-Wall -Winline -ggdb -lm -MMD -MP -Wno-unused-function -Wextra -Wstrict-prototypes
CFLAGS+=-std=gnu11
CFLAGS+=--specs=nano.specs
CFLAGS+=-ffunction-sections -fdata-sections

LFLAGS:=-T$(PROJ_ROOT)/board/$(BOARD_TYPE)/$(LD_FILES)
#LFLAGS	+=-nostartfiles -Xlinker --gc-sections
LFLAGS	+=-Xlinker --gc-sections
LFLAGS  +=-Wl,-Map=$(TARGET).map

LINK_LIBS:= $(addprefix -L,$(USER_LIB_INCLUDES))
LINK_LIBS+= $(addprefix -l,$(USER_LIBS))

#LFLAGS  += -flto -fuse-linker-plugin
# Flags for objcopy
CPFLAGS = -Obinary

# flags for objectdump
ODFLAGS = -S

# Uploader tool path.
FLASHER=JFlashLite
JLINKGDB=JLinkGDBServer
# Flags for the uploader program.
FLASHER_FLAGS=

###################################################

.PHONY:all proj debug libs test_suit test_suit_clean flash

all: proj
	$(HEX)   $(TARGET).elf 	$(TARGET).hex
	$(BIN)   $(TARGET).elf  $(TARGET).bin
	$(DUMP) -St $(TARGET).elf >$(TARGET).lst
	$(SIZE)  $(OBJS) $(TARGET).elf $(TARGET).hex

proj: 	$(TARGET).elf

-include $(DEPS)
$(PROJ_ROOT)/$(OBJDIR)/%.o: $(PROJ_ROOT)/%.c
	@echo Compiling $<...
	@mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) ${<} -o ${@}

$(PROJ_ROOT)/$(OBJDIR)/%.o: $(PROJ_ROOT)/%.s
	@echo Compiling $<...
	@mkdir -p $(dir $@)
	$(AS) -c $(CFLAGS) $< -o $@

$(TARGET).elf: $(OBJS)
	@echo Linking...
	$(CC) $(CFLAGS) $(LFLAGS) -o ${TARGET}.elf $(OBJS) $(LINK_LIBS)

clean:
	find ./ -name '*~' | xargs rm -f	
	rm -f *.o
	rm -f $(OBJS)
	rm -f $(DEPS)
	rm -f $(OBJDIR)/*.o
	rm -f $(OBJDIR)/*.d
	rm -f $(TARGET).elf
	rm -f $(TARGET).hex
	rm -f $(TARGET).bin
	rm -f $(TARGET).map
	rm -f $(TARGET).lst

libs:
	make -C libs/selex-libc/canopen_clib all
test_suit:
	make -C libs/selex-libc/test-suit/ -f test_suit.mk all
test_suit_clean:
	make -C libs/selex-libc/test-suit/ -f test_suit.mk clean
	
flash:
	./flasher.sh
