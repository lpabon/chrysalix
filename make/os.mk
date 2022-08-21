#
# Make rules for kernel
#
INCFLAGS = -I. \
		   -I$(CX_SRC)/include \
	   	   -I$(CX_SRC)/include/$(CX_ARCH)/$(CX_PLATFORM) \
		   -I$(CX_SRC)/sys/include \
		   -I$(CX_SRC)/packages/include \
		   -I$(CX_SRC)/tests/include

ASFLAGS = $(INCFLAGS)
CFLAGS = $(INCFLAGS)
CPPFLAGS = $(INCFLAGS)
include $(CX_SRC)/sys/$(CX_ARCH)/$(CX_PLATFORM)/Makefile.inc
include $(CX_SRC)/make/common.mk
