###############################################################################
###############################################################################

.PHONY: all build menuconfig xconfig clean

-include .config

OUTDIR = build
INSTALLDIR = libdir

###############################################################################

ROOTDIR := $(notdir $(shell pwd))

CROSS =

CPP	= @echo " g++ $@"; $(CROSS)g++
CC	= @echo " gcc $@"; $(CROSS)gcc
LD	= @echo " ld  $@"; $(CROSS)ld
AR  = @echo " ar  $@"; $(CROSS)ar
RM	= @echo " RM	$@"; rm -rf
STRIP	= @echo " strip  $@"; $(CROSS)strip
OBJCOPY = @echo " objcopy $@"; $(CROSS)objcopy
SIZE = "@echo " size $@"; $(CROSS)size


export INSTALLDIR ROOTDIR OUTDIR CPP CC LD AR RM STRIP OBJCOPY SIZE

###############################################################################


MAKEFLAGS += -j --no-print-directory

###############################################################################

###############################################################################

make_target += core json

###############################################################################

all:
	@test -f .config || $(MAKE) -f scripts/Makefile menuconfig
	@$(MAKE) -f Makefile build

build:	
	@for val in `echo $(make_target) | cut -d ' ' -f 1-`;do \
		$(MAKE) -C $$val; \
	done	


core:	
	@$(MAKE) -f core/Makefile all

json:	
	@$(MAKE) -f json/Makefile all


menuconfig:
	@$(MAKE) -f scripts/Makefile $@

xconfig:	
	@$(MAKE) -f scripts/Makefile $@

clean:
	@$(MAKE) -f scripts/Makefile $@
	rm -rf $(OUTDIR)


# Dependencies
-include $(OBJECTS:.o=.d)

###############################################################################
