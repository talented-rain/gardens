#
# Makefile
#
# File Name:   Makefile
# Author:      Yang Yujun
# E-mail:      <yujiantianhu@163.com>
# Created on:  2024.04.12
#
# Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
#

all:

dir-y	:=
include Makefile.include

dir-y	+=	$(patsubst lib%.a,%, $(lib-y))

LIB_LOCAL_PATH  	:=  $(shell pwd)
LIB_OBJECT_PATH 	:=  $(LIB_LOCAL_PATH)/objects
LIB_BUILD_SCRIPT	:= 	$(LIB_LOCAL_PATH)/Makefile.build

ifneq ($(LIB_OBJECT_PATH),$(EXT_LIB_EXEC))
	$(error "library path is not matched!")
endif

export LIB_LOCAL_PATH LIB_OBJECT_PATH LIB_BUILD_SCRIPT

define build_recusion
	@for dir in $(dir-y); do $(Q)$(MAKE) -C $$dir -f $(LIB_BUILD_SCRIPT) $(1); done
endef

_force:
.PHONY:	all clean _force

all: _force
	$(call build_recusion, all)

clean:
	$(call build_recusion, clean)

distclean:
	$(call build_recusion, distclean)
	if [ -d $(LIB_OBJECT_PATH) ]; then rm -rf $(LIB_OBJECT_PATH); fi

debug:
	@echo 'lib Makefile: '$(dir-y)
	$(call build_recusion, debug)

# end of file
