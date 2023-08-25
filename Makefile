# Cross toolchain variables
# If these are not in your path, you can make them absolute.
XT_PRG_PREFIX = mipsel-linux-gnu-
CC = $(XT_PRG_PREFIX)gcc
LD = $(XT_PRG_PREFIX)ld

SRC_DIR = pandos
BUILD_DIR = build
KERNEL_BUILD = kernel

ALL_FILES := $(wildcard $(SRC_DIR)/**/**/*.c) $(wildcard $(SRC_DIR)/**/*.c) $(wildcard $(SRC_DIR)/*.c)
ALL_HEADER_FILES := $(wildcard $(SRC_DIR)/**/**/*.h) $(wildcard $(SRC_DIR)/**/*.h) $(wildcard $(SRC_DIR)/*.h)

ALL_OBJ_FILES := $(ALL_FILES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
OBJ_FOLDERS:= $(addprefix $(BUILD_DIR)/, $(dir $(ALL_FILES:$(SRC_DIR)/%=%)))

# uMPS3-related paths

# Simplistic search for the umps3 installation prefix.
# If you have umps3 installed on some weird location, set UMPS3_DIR_PREFIX by hand.
ifneq ($(wildcard /usr/bin/umps3),)
	UMPS3_DIR_PREFIX = /usr
else
	UMPS3_DIR_PREFIX = /usr/local
endif

UMPS3_DATA_DIR = $(UMPS3_DIR_PREFIX)/share/umps3
UMPS3_INCLUDE_DIR = $(UMPS3_DIR_PREFIX)/include/umps3

# Compiler options
CFLAGS_LANG = -ffreestanding
CFLAGS_MIPS = -mips1 -mabi=32 -mno-gpopt -EL -G 0 -mno-abicalls -fno-pic -mfp32
CFLAGS = $(CFLAGS_LANG) $(CFLAGS_MIPS) -I$(UMPS3_INCLUDE_DIR) -Ipandos/phase1/include -Ipandos/stdlib -Ipandos/phase2/include -O0 -std=c99

# Linker options
LDFLAGS = -G 0 -T $(UMPS3_DATA_DIR)/umpscore.ldscript -m elf32ltsmip

# Add the location of crt*.S to the search path
VPATH = $(UMPS3_DATA_DIR)

.PHONY : all clean

all : kernel.core.umps

run: all
	umps3 Test

kernel : $(ALL_OBJ_FILES) crtso.o libumps.o 
	$(LD) -o $@ $^ $(LDFLAGS)

kernel.core.umps : kernel
	umps3-elf2umps -k $<

clean :
	-rm -rf *.o $(BUILD_DIR) kernel kernel.*.umps
		
crtso.o : crtso.S
	$(CC) $(CFLAGS) -c -o $@ $<

libumps.o : libumps.S
	$(CC) $(CFLAGS) -c -o $@ $<

$(ALL_OBJ_FILES):  $(BUILD_DIR)/%.o : $(SRC_DIR)/%.c | $(OBJ_FOLDERS) 
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_FOLDERS):
	mkdir -p $@

docs: 
	doxygen docfile
rm-docs: 
	rm -rf docs
open_docs:
	xdg-open docs/html/index.html