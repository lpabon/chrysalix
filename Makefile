export CX_SRC=$(shell pwd)
export CX_OS=chrysalix
export CX_BUILD=$(CX_SRC)/build

export CX_ARCH = ucontext
export CX_PLATFORM = linux

OS=$(shell uname -s)
BUILD=DEBUG # or RELEASE
SUBDIRS = sys packages build

# Packages
export CX_PKG_HELLOWORLD=yes
export CX_PKG_TESTS=yes

ifeq ($(OS),Darwin)
export CFLAGS += -D_XOPEN_SOURCE
endif

all: build

ds:
	$(MAKE) \
		CX_ARCH=pth \
		CX_PLATFORM=linux

build: sys packages

packages: sys

run: build
	./build/chrysalixos

include $(CX_SRC)/make/common.mk
