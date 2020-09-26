PREFIX		= mipsel-unknown-elf-

ifndef GCC_VERSION

GCC_VERSION	= 7.4.0

endif	# GCC_VERSION

# PSn00bSDK library/include path setup
ifndef PSN00BSDK_LIBS

# Default assumes libpsn00b is just in the parent dir of the examples dir

LIBDIRS		= -LD:/PSn00bSDK-master/libpsn00b
INCLUDE	 	= -ID:/PSn00bSDK-master/libpsn00b/include

else

LIBDIRS		= -L$(PSN00BSDK_LIBS)
INCLUDE		= -I$(PSN00BSDK_LIBS)/include

endif 		# PSN00BSDK_LIBS

# PSn00bSDK toolchain path setup
ifndef GCC_BASE

ifndef PSN00BSDK_TC

# Default assumes GCC toolchain is in root of C drive or /usr/local

ifeq "$(OS)" "Windows_NT"

GCC_BASE	= /d/mipsel-unknown-elf
GCC_BIN		=

else

GCC_BASE	= /usr/local/mipsel-unknown-elf
GCC_BIN		=

endif

else

GCC_BASE	= $(PSN00BSDK_TC)
GCC_BIN		= $(PSN00BSDK_TC)/bin/

endif		# PSN00BSDK_TC

endif		# GCC_BASE

CC			= $(GCC_BIN)$(PREFIX)gcc
CXX			= $(GCC_BIN)$(PREFIX)g++
AS			= $(GCC_BIN)$(PREFIX)as
AR			= $(GCC_BIN)$(PREFIX)ar
RANLIB		= $(GCC_BIN)$(PREFIX)ranlib

# Project target name
TARGET		= chip84.elf

# Searches for C, C++ and S (assembler) files in local directory
CFILES		= $(notdir $(wildcard *.c))
CPPFILES 	= $(notdir $(wildcard *.cpp))
AFILES		= $(notdir $(wildcard *.s))

# Determine object files
OFILES		= $(addprefix build/,$(CFILES:.c=.o)) \
			$(addprefix build/,$(CPPFILES:.cpp=.o)) \
			$(addprefix build/,$(AFILES:.s=.o))

# Project specific include and library directories
# (use -I for include dirs, -L for library dirs)
INCLUDE	 	+= -ID:/PSn00bSDK-master/libpsn00b/lzp
LIBDIRS		+=

# Libraries to link
LIBS		= -llzp -lpsxgpu -lpsxgte -lpsxspu -lpsxetc -lpsxapi -lc

# C compiler flags
CFLAGS		= -g -O2 -fno-builtin -fdata-sections -ffunction-sections

# C++ compiler flags
CPPFLAGS	= $(CFLAGS) -fno-exceptions

# Assembler flags
AFLAGS		= -g -msoft-float

# Linker flags
LDFLAGS		= -g -Ttext=0x80010000 -gc-sections \
			-T $(GCC_BASE)/mipsel-unknown-elf/lib/ldscripts/elf32elmip.x

# Toolchain programs
CC			= $(PREFIX)gcc
CXX			= $(PREFIX)g++
AS			= $(PREFIX)as
LD			= $(PREFIX)ld

all: resources $(OFILES)
	$(LD) $(LDFLAGS) $(LIBDIRS) $(OFILES) $(LIBS) -o $(TARGET)
	elf2x -q $(TARGET)
	
build/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@
	
build/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(AFLAGS) $(INCLUDE) -c $< -o $@
	
build/%.o: %.s
	@mkdir -p $(dir $@)
	$(CC) $(AFLAGS) $(INCLUDE) -c $< -o $@
	
resources:
	lzpack data.xml
	touch data.s
	
iso:
	mkpsxiso -y -o "Chip-84 PSX Edition.bin" iso.xml
	
clean:
	rm -rf build *.lzp $(TARGET) $(TARGET:.elf=.exe) $(TARGET:.elf=.iso)

run:
	"D:\NOCASHPSX\NOCASHPSX.EXE" $(TARGET:.elf=.exe) &