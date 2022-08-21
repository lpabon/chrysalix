SUBDIRS = drivers
TARGET = $(CX_SRC)/build/kern.a
TYPE = LIBRARY
OBJS = main.o \
       reset.o \
	   cx_drv_arch.o \
	   arch_context.o \
	   linux_context.o

include $(CX_SRC)/make/os.mk
