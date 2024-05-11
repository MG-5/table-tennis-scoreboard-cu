# Makefile for AVR C++ projects
# From: https://gist.github.com/rynr/72734da4b8c7b962aa65

# ----- Update the settings of your project here -----

# Hardware
MCU     = attiny1634 # see `make show-mcu`
F_CPU   = 7372800UL
PROJECT = tabletennis-scoreboard-cu

#Progammer
PROGRAMMER = arduino
PORT = -P/dev/serial/by-id/usb-Arduino__www.arduino.cc__0043_554343437333511141D0-if00 
BAUD = 

# ----- These configurations are quite likely not to be changed -----

# Binaries
GCC     = avr-gcc
G++     = avr-g++
OBJCOPY = avr-objcopy
RM      = rm -f
AVRDUDE = avrdude
AVRSIZE = avr-size

# Files
EXT_C   = c
EXT_CPP = cpp
EXT_CXX = cxx
EXT_ASM = asm

OUTPUT_DIR	:= ./build
OBJDIR		:= ./build/obj
SRCDIR		:= ./src

SOURCES	=  $(shell find $(SRCDIR) -name '*.$(EXT_C)')
SOURCES	+= $(shell find $(SRCDIR) -name '*.$(EXT_CPP)')
SOURCES	+= $(shell find $(SRCDIR) -name '*.$(EXT_CXX)')
SOURCES	+= $(shell find $(SRCDIR) -name '*.$(asm)')

# ----- No changes should be necessary below this line -----

# Object files
TMP1			:= $(patsubst $(SRCDIR)/%.$(EXT_C), $(OBJDIR)/%.o, $(SOURCES))
TMP2			:= $(patsubst $(SRCDIR)/%.$(EXT_CPP), $(OBJDIR)/%.o, $(TMP1))
OBJECTS			:= $(patsubst $(SRCDIR)/%.$(EXT_CXX), $(OBJDIR)/%.o, $(TMP2))

# TODO explain these flags, make them configurable
CFLAGS = $(INC)
CFLAGS += -Os
CFLAGS += -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -Wl,--gc-sections -ffunction-sections -fdata-sections
CFLAGS += -Wall -Wstrict-prototypes
CFLAGS += -DF_CPU=$(OSC)
CFLAGS += -mmcu=$(MCU)

C++FLAGS = $(INC)
C++FLAGS += -std=c++17
C++FLAGS += -Os
C++FLAGS += -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
C++FLAGS += -Wl,--gc-sections -ffunction-sections
C++FLAGS += -Wall
C++FLAGS += -DF_CPU=$(F_CPU)
C++FLAGS += -mmcu=$(MCU)
C++FLAGS += -I$(SRCDIR)

ASMFLAGS = $(INC)
ASMFLAGS += -Os
ASMFLAGS += -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
ASMFLAGS += -Wl,--gc-sections -ffunction-sections
ASMFLAGS += -Wall -Wstrict-prototypes
ASMFLAGS += -DF_CPU=$(F_CPU)
ASMFLAGS += -x assembler-with-cpp
ASMFLAGS += -mmcu=$(MCU)

TARGET=$(OUTPUT_DIR)/$(PROJECT)

.PHONY: clean help show-mcu
.SECONDARY:

all: hex

hex: $(TARGET).hex

print-%:
	@echo $*=$($*)

%.hex: %.elf
	@$(OBJCOPY) -O ihex -j .data -j .text $(TARGET).elf $(TARGET).hex

$(TARGET).elf: $(OBJECTS)
	@$(GCC) $(CFLAGS) $(OBJECTS) -o $@ $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.$(EXT_C)
	@printf "  CC      $<\n"
	@mkdir -p $(dir $@)
	@$(GCC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.$(EXT_CPP)
	@printf "  CXX     $<\n"
	@mkdir -p $(dir $@)
	@$(G++) $(C++FLAGS) -c $< -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.$(EXT_CXX)
	@printf "  CXX     $<\n"
	@mkdir -p $(dir $@)
	@$(G++) $(C++FLAGS) -c $< -o $@

$(OBJDIR)/%.o : %.$(EXT_ASM)
	@printf "  ASM     $<\n"
	@mkdir -p $(dir $@)
	@$(G++) $(ASMFLAGS) -c $< -o $@

clean:
	$(RM) -rf $(OUTPUT_DIR)

help:
	@echo "usage:"
	@echo "  make <target>"
	@echo ""
	@echo "targets:"
	@echo "  clean     Remove any non-source files"
	@echo "  config    Shows the current configuration"
	@echo "  help      Shows this help"
	@echo "  show-mcu  Show list of all possible MCUs"

config:
	@echo "configuration:"
	@echo ""
	@echo "Binaries for:"
	@echo "  C compiler:   $(GCC)"
	@echo "  C++ compiler: $(G++)"
	@echo "  Programmer:   $(AVRDUDE)"
	@echo "  remove file:  $(RM)"
	@echo ""
	@echo "Hardware settings:"
	@echo "  MCU: $(MCU)"
	@echo "  OSC: $(OSC)"
	@echo ""
	@echo "Defaults:"
	@echo "  C-files:   *.$(EXT_C)"
	@echo "  C++-files: *.$(EXT_CPP) & *.$(EXT_CXX)" 
	@echo "  ASM-files: *.$(EXT_ASM)"

show-mcu:
	$(G++) --help=target

size:
	avr-size --mcu=$(MCU) -C $(TARGET).elf

flash: hex
	avrdude -p$(MCU) $(PORT) $(BAUD) -c$(PROGRAMMER) -Uflash:w:$(TARGET).hex:a
