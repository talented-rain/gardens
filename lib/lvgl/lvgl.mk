#
# Makefile
#
# File Name:   Makefile
# Author:      Yang Yujun
# E-mail:      <yujiantianhu@163.com>
# Created on:  2024.11.16
#
# Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
#

LVGL_DIR		:=	$(LIB_LOCAL_PATH)
LVGL_DIR_NAME	:= 	lvgl

CFLAGS		:=

EXTRA_FLAGS	+= 	-finput-charset=GBK
EXTRA_TEMP  := 	$(filter-out -fexec-charset%, $(EXTRA_FLAGS))

ifneq ($(EXTRA_TEMP),)
EXTRA_TEMP  += 	-fexec-charset=GBK
EXTRA_FLAGS := 	$(EXTRA_TEMP)
else
EXTRA_FLAGS += 	-fexec-charset=GBK
endif

CSRCS	:=
include examples/lv_examples.mk
include src/core/lv_core.mk
include src/draw/lv_draw.mk
include src/extra/lv_extra.mk
include src/font/lv_font.mk
include src/hal/lv_hal.mk
include src/misc/lv_misc.mk
include src/widgets/lv_widgets.mk

src-y	+= 	$(CSRCS)

VPATH	+= 	$(LVGL_DIR)/$(LVGL_DIR_NAME)/src/extra
VPATH	:=	$(sort $(VPATH) $(dir $(CSRCS)))
VPATH	:=	$(filter-out ./, $(VPATH))

inc-y	+= 	-I $(LVGL_DIR)/$(LVGL_DIR_NAME)/src
inc-y	+= 	$(foreach dir, $(CFLAGS), $(patsubst "%",%, $(dir)))

# end of file
