#
# Make rule for ARCH
#
INCFLAGS = -I. \
		   -I$(CX_SRC)/include \
	   	   -I$(CX_SRC)/include/$(CX_ARCH)/$(CX_PLATFORM) \
		   -I$(CX_SRC)/sys/include \
           -I$(CX_SRC)/tests/include

ASFLAGS = $(INCFLAGS)
CFLAGS = -Wall $(INCFLAGS)
CPPFLAGS = $(INCFLAGS)
include $(CX_SRC)/bsp/arch/$(CX_ARCH)/$(CX_PLATFORM)/Makefile.inc
include $(CX_SRC)/make/common.mk
