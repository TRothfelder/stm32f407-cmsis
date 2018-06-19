######################################
# target
######################################
#name of the binary
TARGET = blinky


######################################
# building variables
######################################
# Build path
BUILD_DIR = build

# where to the binaries in
BIN_DIR = bin

# debug build?
DEBUG = true

# optimization
OPT = -Os

#if it is a debug build, don't optimize
ifeq ($(DEBUG), true)
	OPT = -O0
endif


######################################
# Setup of source files
######################################
# c sources relatively to this makefile, just add custom source files here
C_SOURCES  = src/main.c
C_SOURCES += src/vector.c

# c includes relatively to this makefile
C_INCLUDES_DIR  = inc
C_INCLUDES_DIR += inc/CMSIS/Include
C_INCLUDES_DIR += inc/CMSIS/Device/ST/STM32F4xx/Include

#don't touch, adds the -I prefix for the c Preprocessor
C_INCLUDES_DIR := $(addprefix -I,$(C_INCLUDES_DIR))

# assembly files to be included
ASM_SOURCES =



#######################################
# CFLAGS
#######################################
# Architecture configuration
ARCH_FLAGS = -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16

# C defines
C_DEFS  = STM32F40_41xxx HSE_VALUE="((uint32_t)8000000)"

#if debug add DEBUG define
ifeq ($(DEBUG),true)
	C_DEFS += DEBUG
endif

# C Flags
CFLAGS += $(OPT)
CFLAGS += $(ARCH_FLAGS)
CFLAGS += -MD -std=c99
CFLAGS += -Wall -Wextra -Wimplicit-function-declaration -Wredundant-decls
CFLAGS += -Wstrict-prototypes -Wundef -Wshadow -Wmissing-prototypes
CFLAGS += -fno-common
CFLAGS += $(C_INCLUDES_DIR)
CFLAGS += $(addprefix -D, $(C_DEFS))

#include debug symbols for gdb in debug build, use -g for other debugggers
ifeq ($(DEBUG), true)
	CFLAGS += -ggdb3
endif



#######################################
# LDFLAGS
#######################################
# link script
#this script is for the stm32f4 discovery board without c++ support.
#If you use another cortex-m cpu you should be fine with editing the ram and flash size in this file.
LDSCRIPT = stm32f407.ld

# add libraries here
LIBS = c m nosys
# custom paths to search for libs
LIBDIR = 

#linker flags
LDFLAGS  = $(ARCH_FLAGS)
LDFLAGS += -T$(LDSCRIPT)
LDFLAGS += -nostartfiles
LDFLAGS += -Wl,--gc-sections
LDFLAGS += -Wl,-Map=$(BUILD_DIR)/$(TARGET).map
#-nostartfiles ist der übeltäter für das nicht compilen ohne .eh_ section im linkerscript
LDFLAGS += $(addprefix -L,$(LIBDIR))
LDFLAGS += $(addprefix -l,$(LIBS))



#######################################
# compiler binaries
#######################################
#use a specific compiler instead of the one in the path
#BINPATH = /mnt/luksdata/opt/gcc-arm-none-eabi/6-2017-q2/bin

#the prefix of the compiler. just leave it as it is. this fits all arm cortex processors
PREFIX = arm-none-eabi-

#create variabls for compiler linker, etc.
CC = $(if $(BINPATH),$(BINPATH)/,)$(PREFIX)gcc
LD = $(if $(BINPATH),$(BINPATH)/,)$(PREFIX)gcc
AS = $(if $(BINPATH),$(BINPATH)/,)$(PREFIX)gcc -x assembler-with-cpp
CP = $(if $(BINPATH),$(BINPATH)/,)$(PREFIX)objcopy
AR = $(if $(BINPATH),$(BINPATH)/,)$(PREFIX)ar
SZ = $(if $(BINPATH),$(BINPATH)/,)$(PREFIX)size



#######################################
# Create tree of object files
#######################################
OBJECTS =

#add object files for the c source files to the tree
ifneq ($(C_SOURCES), )
	C_OBJECTS = $(C_SOURCES:.c=.o)
	OBJECTS += $(C_OBJECTS)
else
	C_OBJECTS =
endif

#add object files for the asm source files to the tree
ifneq ($(ASM_SOURCES), )
	ASM_OBJECTS = $(ASM_SOURCES:.s=.o)
	OBJECTS += $(ASM_OBJECTS)
else
	ASM_OBJECTS =
endif

#create path tree of the object files, later used to create the paths
OBJECTS_DIR_TREE = $(foreach d, $(sort $(dir $(OBJECTS))), $(d))

#create deps from the object files
DEPS = $(addprefix $(BUILD_DIR)/,$(OBJECTS:.o=.d))

#######################################
# build the application
#######################################
.PHONY: all clean
all: $(BUILD_DIR)/$(TARGET).elf

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR) $(addprefix $(BUILD_DIR)/,$(OBJECTS_DIR_TREE))
	@echo "  CC $<"
	$(CC) $(CFLAGS) -o $@ -c $<

$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR) $(addprefix $(BUILD_DIR)/,$(OBJECTS_DIR_TREE))
	@echo "  AS $<"
	$(AS) $(CFLAGS) -o $@ -c $<

$(BUILD_DIR)/$(TARGET).elf: $(addprefix $(BUILD_DIR)/,$(OBJECTS)) Makefile | $(BUILD_DIR) $(addprefix $(BUILD_DIR)/,$(OBJECTS_DIR_TREE)) $(BIN_DIR)
	@echo "  LINK $@"
	$(CC) -o $@ $(addprefix $(BUILD_DIR)/,$(OBJECTS)) $(LDFLAGS)
	@echo "  Copy executables to binary path"
	$(CP) -O ihex $< $(BIN_DIR)/$(TARGET).hex
	$(CP) -O binary -S $< $(BIN_DIR)/$(TARGET).bin
	cp $(BUILD_DIR)/$(TARGET).elf $(BIN_DIR)/$(TARGET).elf
	cp $(BUILD_DIR)/$(TARGET).map $(BIN_DIR)/$(TARGET).map
	$(SZ) $(BIN_DIR)/$(TARGET).elf



$(addprefix $(BUILD_DIR)/,$(OBJECTS_DIR_TREE)):
	@echo "  MKDIR"
	mkdir -p $@

$(BUILD_DIR):
	@echo "  MKDIR"
	mkdir -p $@

$(BIN_DIR):
	@echo "  MKDIR"
	mkdir -p $@

clean:
	@echo "  RM"
	rm -fR $(DEPS)
	rm -fR $(BUILD_DIR)
	rm -fR $(BIN_DIR)

# include dependency files (*.d)
-include $(DEPS)
