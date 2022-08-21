#
# common.mk - common make rules to build Prex
#

# Required shell variables
#
#  CX_SRC      ... Root directory of source tree
#  CX_ARCH     ... Architecture name
#  CX_PLATFORM ... Platform name
#  CX_OS	   ... OS to compile
#
# Optional shell variables
#
#  NDEBUG	... 0 for debug, 1 for release (default: 0)
#  CROSS_COMPLE ... Prefix for tools
#

# Variables in local Makefile
#
#  TARGET	... Target file name
#  TYPE		... Traget file type
#		    ex. OBJECT,LIBRARY,KERNEL,BINARY,DRIVER,OS_IMAGE
#  SUBDIRS	... List of subdirectories
#  OBJS		... Object files
#  LIBS		... Libraries
#  MAP		... Name of map file
#  DISASM	... Disassemble list file
#  SYMBOL	... Symbol files

#
# Option for cross compile
#
#CROSS_COMPILE	=
#CROSS_COMPILE	= i386-elf-
#CROSS_COMPILE	= arm-elf-
#CROSS_COMPILE	= powerpc-eabi-
#CROSS_COMPILE	= sh-elf-

#
# Set cross-compiler (This can be done better)
#
ifeq ($(CX_ARCH),arm)
CROSS_COMPILE	= arm-elf-
endif

ifeq ($(CX_ARCH),pth)
CROSS_COMPILE   =
endif


OS=$(shell uname -s)

#
# Tools
#
ifeq ($(OS),Darwin)
CC	= $(CROSS_COMPILE)clang
else
CC	= $(CROSS_COMPILE)gcc
endif
CPP	= $(CROSS_COMPILE)cpp
AS	= $(CROSS_COMPILE)as
LD	= $(CROSS_COMPILE)ld
AR	= $(CROSS_COMPILE)ar
OBJCOPY	= $(CROSS_COMPILE)objcopy
OBJDUMP	= $(CROSS_COMPILE)objdump
STRIP	= $(CROSS_COMPILE)strip

#
# INDENT
#
INDENT	= indent
INDENT_OPTS = -kr -ts4 -i4 -nut -brf

ifdef DISASM
ASMGEN	= $(OBJDUMP) $@ --disassemble-all > $(DISASM)
endif

#
# Flags
#

DEF_FLAGS = -D__$(OS)__ -D__$(CX_ARCH)__ -D__$(CX_PLATFORM)__ -D__$(CX_OS)__ \
			-D__ARCH__=\"$(CX_ARCH)\" -D__PLATFORM__=\"$(CX_PLATFORM)\" \
			-D__CX_OS__=\"$(CX_OS)\" \
			-U$(CX_ARCH) -U$(CX_PLATFORM)

ifeq ($(BUILD),RELEASE)
CFLAGS += -O2 -fomit-frame-pointer
else
CFLAGS += -DDEBUG -g -O0
endif

ifdef MAP
LDFLAGS += -Map $(MAP)
endif

#
# Specify path for libgcc.a
#
#LIBGCC_PATH = /prex/lib/arm/
ifndef LIBGCC_PATH
LIBGCC_PATH := $(dir $(shell $(CC) $(GLOBAL_CFLAGS) -print-libgcc-file-name))
endif

ASFLAGS	+=
CFLAGS	+= -pipe -fno-strict-aliasing $(DEF_FLAGS)
CPPFLAGS += $(DEF_FLAGS)
LDFLAGS	+= -L$(CX_BUILD)


#
# Inference rules
#
%.o: %.c
	@echo "    CC $<"
	@$(CC) $(CFLAGS) $(EXTRA_CFLAGS) -c -o $@ $<

%.o: %.s
	@echo "    AS $<"
	@$(AS) $(ASFLAGS) $(EXTRA_ASFLAGS) -o $@ $<

%.o: %.S
	@echo "    CPP/AS $<"
	@$(CPP) $(CPPFLAGS) $(EXTRA_CPPFLAGS) $< $*.tmp
	@$(AS) $(ASFLAGS) $(EXTRA_ASFLAGS) -o $@ $*.tmp
	@rm -f $*.tmp

#
# Target
.PHONY: all
all: $(SUBDIRS) $(TARGET)

#
# Rules to process sub-directory
#
ifdef SUBDIRS
.PHONY: $(SUBDIRS)
$(SUBDIRS):
	$(MAKE) -C $@

$(OBJS): $(SUBDIRS)

endif


#
# Rules to compile a set of .o files into one .o file
#
ifeq ($(TYPE),OBJECT)
$(TARGET): $(OBJS)
	@echo "    LD $@"
	#$(LD) $(LDFLAGS) $(EXTRA_LDFLAGS) -r -o $@ $(OBJS) $(LIBS)
	#$(AR) rcs $@ $(OBJS)
endif

#
# Rules to compile library
#
ifeq ($(TYPE),LIBRARY)
$(TARGET): $(OBJS) ar-target

ar-target: $(OBJS)
	@echo "    AR $(TARGET)"
	@$(AR) $(EXTRA_ARFLAGS) rcs $(TARGET) $?
endif

#
# Rules to compile kernel
#
ifeq ($(TYPE),KERNEL)
$(TARGET): $(OBJS) $(LIBS)
	$(LD) $(LDFLAGS) -T $(LD_SCRIPT) -o $@ $(OBJS) $(LIBS)
	$(ASMGEN)
ifdef SYMBOL
	cp prex $(SYMBOL)
endif
	$(STRIP) $@
endif

#
# Rules to compile device driver
#
ifeq ($(TYPE),DRIVER)
$(TARGET):  $(OBJS)
	$(LD) $(LDFLAGS) -T $(LD_SCRIPT) -o $@ $(OBJS) $(LIBS)
	$(ASMGEN)
ifdef SYMBOL
	cp $@ $@.sym
endif
	$(STRIP) $@
endif

#
# Rules to compile binary file
#
ifeq ($(TYPE),BINARY)
$(TARGET): $(OBJS)
	$(LD) $(LDFLAGS) $(EXTRA_LDFLAGS) -o $@.elf $(OBJS) $(LIBS) -Map=$@.map
	$(ASMGEN)
	$(OBJDUMP) -Sd $@.elf > $@.lst
	$(OBJDUMP) -d --section=.rodata --section=.bss $@.elf >> $@.lst

	$(OBJCOPY) $(OBJCOPYFLAGS) $@.elf $@.bin
endif

#
# Rules to compile binary file
#
ifeq ($(TYPE),INTEL_HEX)
$(TARGET): $(OBJS)
	@echo "    Creating $@.hex"
	$(LD) $(LDFLAGS) $(EXTRA_LDFLAGS) -o $@.elf $(OBJS) $(LIBS) -Map=$@.map
	$(ASMGEN)
	$(OBJDUMP) -Sd $@.elf > $@.lst
	$(OBJDUMP) -d --section=.rodata --section=.bss $@.elf >> $@.lst

	$(OBJCOPY) $(OBJCOPYFLAGS) $@.elf $@.hex
endif

#
# Rules to compile executable file
#
ifeq ($(TYPE),EXEC)
$(TARGET): $(OBJS) $(LIBS)
	$(LD) $(LDFLAGS) $(EXTRA_LDFLAGS) -T $(LD_SCRIPT) \
	-o $@ $(CX_SRC)/user/lib/crt0.o $(OBJS) \
	$(CX_SRC)/user/lib/libc.a $(CX_SRC)/user/lib/libprex.a \
	$(LIBGCC_PATH)libgcc.a
	$(ASMGEN)
	$(STRIP) $@
endif

#
# Rules to create OS image
#
ifeq ($(TYPE),OS_IMAGE)
$(TARGET): dummy
	$(AR) rcS tmp.a $(KERNEL) $(DRIVER) $(TASK)
	$(AR) t tmp.a
	cat $(LOADER) tmp.a > $@
	rm tmp.a
endif

#
# Rules to create an executable program.
# Normally used for the kernel to run on top
# of another OS, as a process.
#
ifeq ($(TYPE),OS_EXECPROGRAM)
$(TARGET): dummy
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) $(INCLUDE) $(LDFLAGS) $(EXTRA_LDFLAGS) $(LIB_OPTIONS) $(LIBS) -o $(TARGET) $(OBJS)
endif


-include Makefile.dep

#
# Depend
#
SRCS = $(wildcard *.c) $(wildcard *.S) $(wildcard *.arms)

.PHONY: depend
depend:
ifdef SUBDIRS
	@(for d in $(SUBDIRS) _ ; do \
	  if [ "$$d" != "_" ] ; then $(MAKE) -C $$d depend; fi; \
	done);
endif
	rm -f Makefile.dep
	@(for d in $(SRCS) _ ; do \
	  if [ "$$d" != "_" ] ; then \
	  $(CPP) -M $(CPPFLAGS) $$d >> Makefile.dep; fi; \
	done);

#
# Indent to "standard"
#
.PHONY: indent
indent:
ifdef SUBDIRS
	@(for d in $(SUBDIRS) _ ; do \
	  if [ "$$d" != "_" ] ; then $(MAKE) -C $$d indent; fi; \
	done);
endif
	@(for d in $(SRCS) _ ; do \
	  if [ "$$d" != "_" ] ; then \
	  $(INDENT) $(INDENT_OPTS) $$d; fi; \
	done);


#
# Clean up
#
.PHONY: clean
clean:
ifdef SUBDIRS
	@(for d in $(SUBDIRS) _ ; do \
	  if [ "$$d" != "_" ] ; then $(MAKE) -C $$d clean; fi; \
	done);
endif
	rm -f Makefile.dep
	rm -f *.c~
	rm -f $(OBJS)
	rm -f $(TARGET)
ifdef DISASM
	rm -f $(DISASM)
endif
ifdef MAP
	rm -f $(MAP)
endif
ifdef SYMBOL
	rm -f $(SYMBOL)
endif
ifdef CLEANS
	rm -f $(CLEANS)
endif
ifdef CLEANDIR
	rm -rf $(CLEANDIR)
endif

#
# Distclean cleans all code and libraries
#
distclean: clean
ifdef SUBDIRS
	@(for d in $(SUBDIRS) _ ; do \
	  if [ "$$d" != "_" ] ; then $(MAKE) -C $$d distclean; fi; \
	done);
endif
	rm -f Makefile.dep
	rm -f $(OBJS)
	rm -f $(TARGET)
ifdef DISASM
	rm -f $(DISASM)
endif
ifdef MAP
	rm -f $(MAP)
endif
ifdef SYMBOL
	rm -f $(SYMBOL)
endif
ifdef CLEANS
	rm -f $(CLEANS)
endif
ifdef CLEANDIR
	rm -rf $(CLEANDIR)
endif

#
# Tests
#
#tests: $(SUBDIRS) $(TARGET)


.PHONY: dummy
dummy:
