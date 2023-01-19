### Normal C compilation
#
#SRC_DIR = src
#INT_DIR = build
#TARGET = phase_1
#
#CXX = gcc
#
#CXXFLAGS = -Wall -Wextra -Werror -pedantic -std=c99 -g
#
#ALL_FILES := $(wildcard $(SRC_DIR)/**/**/*.c) $(wildcard $(SRC_DIR)/**/*.c) $(wildcard $(SRC_DIR)/*.c)
#ALL_HEADER_FILES := $(wildcard $(SRC_DIR)/*.h) $(wildcard $(SRC_DIR)/**/*.h) $(wildcard $(SRC_DIR)/**/**/*.h)
#CPP_FILES := $(filter-out %.test.cpp, $(ALL_FILES))
#
#ALL_OBJ_FILES := $(ALL_FILES:$(SRC_DIR)/%.c=$(INT_DIR)/%.o)
#C_OBJ_FILES := $(filter %.o, $(ALL_OBJ_FILES))
#
#all: info $(TARGET)
#
#info: 
#	@echo "SRC_DIR: $(SRC_DIR)"
#	@echo "INT_DIR: $(INT_DIR)"
#	@echo "TARGET: $(TARGET)"
#	@echo "CXX: $(CXX)"
#	@echo "CXXFLAGS: $(CXXFLAGS)"
#	@echo "ALL_FILES: $(ALL_FILES)"
#	@echo "ALL_HEADER_FILES: $(ALL_HEADER_FILES)"
#	@echo "ALL_OBJ_FILES: $(ALL_OBJ_FILES)"
#
#clean:
#	@echo -e "RMRG\build $(TARGET)"
#	@rm -rf build $(TARGET)
#
#run: all
#	@echo -e "RUN\t$(TARGET)"
#	@./$
#
#$(TARGET): $(C_OBJ_FILES)
#	@echo -e "LD\t$@"
#	@$(CXX) $^ $(LDFLAGS) -o $@


#### Compile of UMPS3
#
## Cross toolchain variables
#XT_PRG_PREFIX = mipsel-linux-gnu-
#CC = $(XT_PRG_PREFIX)gcc
#LD = $(XT_PRG_PREFIX)ld
#
## uMPS3-related paths
#
## Simplistic search for the umps3 installation prefix.
## If you have umps3 installed on some weird location, set UMPS3_DIR_PREFIX by hand.
#ifneq ($(wildcard /usr/bin/umps3),)
#	UMPS3_DIR_PREFIX = /usr
#else
#	UMPS3_DIR_PREFIX = /usr/local
#endif
#
#UMPS3_DATA_DIR = $(UMPS3_DIR_PREFIX)/share/umps3
#UMPS3_INCLUDE_DIR = $(UMPS3_DIR_PREFIX)/include/umps3
#
## Compiler options
#CFLAGS_LANG = -ffreestanding -ansi
#CFLAGS_MIPS = -mips1 -mabi=32 -mno-gpopt -EL -G 0 -mno-abicalls -fno-pic -mfp32
#CFLAGS = $(CFLAGS_LANG) $(CFLAGS_MIPS) -I$(UMPS3_INCLUDE_DIR) -Wall -O0
#
## Linker options
#LDFLAGS = -G 0 -nostdlib -T $(UMPS3_DATA_DIR)/umpscore.ldscript -m elf32ltsmip
#
## Add the location of crt*.S to the search path
#VPATH = $(UMPS3_DATA_DIR)
#
#.PHONY : all clean
#
#all : kernel.core.umps
#
#kernel.core.umps : kernel
#	umps3-elf2umps -k $<
#
#kernel : ash.o crtso.o libumps.o
#	$(LD) -o $@ $^ $(LDFLAGS)
#
#clean :
#	-rm -f *.o kernel kernel.*.umps
#
## Pattern rule for assembly modules
#%.o : %.S
#	$(CC) $(CFLAGS) -c -o $@ $<





# Directory of where to build
SRC_DIR = pandos/phase1 pandos/testers
INT_DIR = build

# Wildcards for files
ALL_FILES := $(wildcard $(SRC_DIR)/**/**/*.c) $(wildcard $(SRC_DIR)/**/*.c) $(wildcard $(SRC_DIR)/*.c)
ALL_HEADER_FILES := $(wildcard $(SRC_DIR)/*.h) $(wildcard $(SRC_DIR)/**/*.h) $(wildcard $(SRC_DIR)/**/**/*.h)

ALL_OBJ_FILES := $(ALL_FILES:$(SRC_DIR)/%.c=$(INT_DIR)/%.o)
C_OBJ_FILES := $(filter %.o, $(ALL_OBJ_FILES))


# Cross toolchain variables
# If these are not in your path, you can make them absolute.
XT_PRG_PREFIX = mipsel-linux-gnu-
CC = $(XT_PRG_PREFIX)gcc
LD = $(XT_PRG_PREFIX)ld

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
CFLAGS_LANG = -ffreestanding -ansi
CFLAGS_MIPS = -mips1 -mabi=32 -mno-gpopt -EL -G 0 -mno-abicalls -fno-pic -mfp32
CFLAGS = $(CFLAGS_LANG) $(CFLAGS_MIPS) -I$(UMPS3_INCLUDE_DIR) -Wall -O0

# Linker options
LDFLAGS = -G 0 -nostdlib -T $(UMPS3_DATA_DIR)/umpscore.ldscript -m elf32ltsmip

# Add the location of crt*.S to the search path
VPATH = $(UMPS3_DATA_DIR)

all: info kernel.core.umps clean

# Information about the build
info: 
	@echo "DIRECTORIES:"
	@echo "\tSRC_DIR: $(SRC_DIR)"
	@echo "\tINT_DIR: $(INT_DIR)"
	@echo "\tTARGET: $(TARGET)"
	@echo "TOOLS:"
	@echo "\tCC: $(CC)"
	@echo "\tCFLAGS: $(CFLAGS)"
	@echo "\tLD: $(LD)"
	@echo "\tLDFLAGS: $(LDFLAGS)"
	@echo "FILES:"
	@echo "\tALL_FILES: $(ALL_FILES)"
	@echo "\tALL_HEADER_FILES: $(ALL_HEADER_FILES)"
	@echo "\tALL_OBJ_FILES: $(ALL_OBJ_FILES)"

# Build the kernel
kernel.core.umps : kernel
	umps3-elf2umps -k $<

kernel : $(ALL_OBJ_FILES) $(INT_DIR)/crtso.o $(INT_DIR)/libumps.o
	$(LD) -o $@ $^ $(LDFLAGS)

clean :
	-rm -f *.o 
# ERANO INCLUSE NELLA CANCELLAZIONE: kernel kernel.*.umps

# Pattern rule for assembly modules
$(ALL_OBJ_FILES) : $(ALL_FILES)
	$(CC) $(CFLAGS) -c -o $@ $<

%.o : %.S
	$(CC) $(CFLAGS) -c -o $@ $<