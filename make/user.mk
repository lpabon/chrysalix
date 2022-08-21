#
# user.mk - make rules to build user mode applications.
#
ASFLAGS = -I$(PREX_SRC)/user/include
CFLAGS = -I$(PREX_SRC)/user/include
CPPFLAGS = -I$(PREX_SRC)/user/include
LDFLAGS = -static
include $(PREX_SRC)/user/arch/$(PREX_ARCH)/Makefile.inc
include $(PREX_SRC)/make/common.mk
