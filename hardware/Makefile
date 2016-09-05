
############################# PROJECT SETUP #############################
# Put your stlink folder here so make burn will work.
STLINK=/home/embed-autograder/stlink/build
#STLINK=/usr/local/bin/stlink

# Add support for Cortex-M4 and STM32F4 Discovery Board
LIB_COMMON = /home/embed-autograder/Workspace/autograder/stm32_libraries/stm32_discovery_arm_gcc/STM32F4-Discovery_FW_V1.1.0
#LIB_COMMON = /home/paul/Workspace/MobileRadar/src/arm/common/stm32f4discovery

# discovery support
DISCOVERY = $(LIB_COMMON)/Libraries


# Project name
PROJ_NAME=AutoGrader
OUTPATH=out

############################# SOURCE DIRECTORIES ##########################
SRC_DIR = .
SRC_DIR += src
SRC_DIR += $(DISCOVERY)/STM32F4xx_StdPeriph_Driver/src


############################# INCLUDE DIRECTORIES ##########################
INC_DIR = .
INC_DIR += header
INC_DIR += $(DISCOVERY)/CMSIS/ST/STM32F4xx/Include
INC_DIR += $(DISCOVERY)/CMSIS/Include
INC_DIR += $(DISCOVERY)/STM32F4xx_StdPeriph_Driver/inc
INC_DIR += $(LIB_COMMON)/Utilities/STM32F4-Discovery

############################# ADDING C FILES #############################
SRCS = $(shell find $(SRC_DIR) -maxdepth 1 -name '*.c' -printf "%f\n")

############################# STARTUP FILE #############################

# add startup file to build
SRCS += src/startup_stm32f40xx.s 

############################# BUILD SETUP #############################
CC=arm-none-eabi-gcc
OBJCOPY=arm-none-eabi-objcopy
SIZE=arm-none-eabi-size

# Choose debug or release...
CFLAGS = -g -O2   # Normal

CFLAGS += -Tstm32_flash.ld
CFLAGS += -DUSE_STDPERIPH_DRIVER
CFLAGS += -mlittle-endian -mthumb -mthumb-interwork -mcpu=cortex-m4
#CFLAGS += -msoft-float

############################# PATH SETUP #############################
vpath %.c $(SRC_DIR) 

# Includes
CFLAGS += $(foreach d, $(INC_DIR), -I$d)
OBJS = $(SRCS:.c=.o)
LDFLAGS = -lm

################################ TARGETS ################################
.PHONY: lib proj dir_tree

all:  dir_tree proj

dir_tree:
	mkdir -p $(OUTPATH)

proj: $(OUTPATH)/$(PROJ_NAME).elf
	$(SIZE) $(OUTPATH)/$(PROJ_NAME).elf

$(OUTPATH)/$(PROJ_NAME).elf: $(SRCS)
	$(CC) $(CFLAGS) $^ -o $@ $(LIBPATHS) $(LIBS) $(LDFLAGS)
	$(OBJCOPY) -O ihex $(OUTPATH)/$(PROJ_NAME).elf $(OUTPATH)/$(PROJ_NAME).hex
	$(OBJCOPY) -O binary $(OUTPATH)/$(PROJ_NAME).elf $(OUTPATH)/$(PROJ_NAME).bin

clean:
	find . -name \*.o -type f -delete
	find . -name \*.lst -type f -delete
	rm -f $(OUTPATH)/$(PROJ_NAME).elf
	rm -f $(OUTPATH)/$(PROJ_NAME).hex
	rm -f $(OUTPATH)/$(PROJ_NAME).bin

# Flash the STM32F4
burn: proj
	$(STLINK)/st-flash write $(OUTPATH)/$(PROJ_NAME).bin 0x8000000


print-%:
	@echo $* = $($*)