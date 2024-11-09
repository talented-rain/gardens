#
# Kernel Top Makefile
#
# File Name:   Makefile
# Author:      Yang Yujun
# E-mail:      <yujiantianhu@163.com>
# Created on:  2023.09.09
#
# Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
#

all:

TARGET			:=	HeavenFox
PROJECT_DIR		:=	$(shell pwd)

# *********************************************************************
CONFIGS			?=

CONF_MAKEFILE	:=	$(PROJECT_DIR)/configs/auto.conf
CONF_HEADER		:=	$(PROJECT_DIR)/configs/autoconf.h

define create_header
	@if [ -e $(1) ]; then rm -f $(1); fi

	@`grep -E "^\s*\w+\s*=\s*" $(2) > $(1)`
	@`sed -i -E 's/\s*=\s*y\s*$$/\ 1/g' $(1)`
	@`sed -i -E 's/\s*=\s*m\s*$$/\ 1/g' $(1)`
	@`sed -i -E '/\s*=\s*n\s*$$/d' $(1)`
	@`sed -i -E 's/\\s*=\s*/\ /g' $(1)`
	@`sed -i -E 's/^\s*/#define\ /g' $(1)`
	@`sed -i -E 's/\s+$$//g' $(1)`
	
	@echo the auto.conf was written to $(CONF_HEADER) !
endef

define create_conf
	@if [ -e $(1) ]; then rm -f $(1); fi
	@`cat $(2) > $(1)`

	@echo configuration was written to $(CONF_MAKEFILE) !
endef

ifeq ($(CONFIGS),)
ifeq ($(wildcard $(CONF_MAKEFILE)),)
	$(error "can not find $(CONF_MAKEFILE)")
endif

CONF_GERNERIC	:=	$(CONF_MAKEFILE)

else
CONF_GERNERIC	:=	$(PROJECT_DIR)/configs/mach/$(CONFIGS)
endif

include $(CONF_GERNERIC)

ifeq ($(CONFIG_ARCH),)
	$(error "undefine CONFIG_ARCH !")
endif

ifeq ($(CONFIG_TYPE),)
	$(error "undefine CONFIG_TYPE !")
endif

ifeq ($(CONFIG_CLASS),)
	$(error "undefine CONFIG_CLASS !")
endif

ifeq ($(CONFIG_CPU),)
	$(error "undefine CONFIG_CPU !")
endif

ifeq ($(CONFIG_VERDOR),)
	$(error "undefine CONFIG_VERDOR !")
endif

ifeq ($(CONFIG_COMPILER),)
	$(error "undefine CONFIG_COMPILER !")
endif

ARCH			?=	$(patsubst "%",%, $(CONFIG_ARCH))
TYPE			?=	$(patsubst "%",%, $(CONFIG_TYPE))
CLASS			?=	$(patsubst "%",%, $(CONFIG_CLASS))
CPU				?=	$(patsubst "%",%, $(CONFIG_CPU))
VENDOR			?=	$(patsubst "%",%, $(CONFIG_VERDOR))
COMPILER		?= 	$(patsubst "%",%, $(CONFIG_COMPILER))

ifneq ($(CONFIG_COMPILER_PATH),)
COMPILER_PATH	?=	$(patsubst "%",%, $(CONFIG_COMPILER_PATH))
else
COMPILER_PATH	?= 	/usr/bin
endif

# *********************************************************************
MAKE			:=	make
Q				:=	

CC				:= 	$(COMPILER)gcc
CXX				:=  $(COMPILER)g++
LD				:= 	$(COMPILER)ld
AR				:=	$(COMPILER)ar
OBJDUMP			:= 	$(COMPILER)objdump
OBJCOPY			:= 	$(COMPILER)objcopy
READELF			:= 	$(COMPILER)readelf

COMPILER_LIBC	:=	$(COMPILER_PATH)/$(COMPILER)/libc

LIBS_PATH		:=  -L $(COMPILER_LIBC)/usr/lib	\
					-L $(COMPILER_LIBC)/lib

LIBS			:=	--static -lgcc -lm -lc
EXTRA_FLAGS		:=  -fexec-charset=GB2312

ifeq ($(CONFIG_VFP),y)
EXTRA_FLAGS     +=	-mcpu=$(CLASS)	\
					-mfpu=vfpv3	\
					-mfloat-abi=hard
endif

BUILD_CFLAGS   	:=  -g3 -O0 -Wall -nostdlib	\
					-Wundef	\
					-Wstrict-prototypes	\
					-Wno-trigraphs \
                    -fno-strict-aliasing	\
					-fno-common \
                    -Werror-implicit-function-declaration \
                    -fno-tree-scev-cprop

MACROS			+=	-DCONFIG_DEBUG_JTAG

# *********************************************************************

OBJECT_PATH		:=	$(PROJECT_DIR)/objects

OUTPUT_PATH		:=	$(PROJECT_DIR)/boot
IMAGE_PATH		:=	$(OUTPUT_PATH)/image
LINK_SCRIPT		:=	$(PROJECT_DIR)/arch/$(ARCH)/cpu/$(TYPE)/$(CPU)/cpu_ramboot.lds
DTC				:=	$(PROJECT_DIR)/scripts/dtc/dtc
BUILD_SCRIPT	:=	$(PROJECT_DIR)/scripts/Makefile.build

TARGET_EXEC		:=	$(IMAGE_PATH)/$(TARGET).elf
TARGET_BINY		:=	$(IMAGE_PATH)/$(TARGET).bin
TARGET_IMGE		:=	$(IMAGE_PATH)/$(TARGET).img
TARGET_NASM		:=	$(IMAGE_PATH)/$(TARGET).dis
TARGET_LASM		:=	$(IMAGE_PATH)/$(TARGET).lst
TARGET_MMAP		:=	$(IMAGE_PATH)/$(TARGET).map

INCLUDE_DIRS	:= 	$(PROJECT_DIR)/	\
					$(PROJECT_DIR)/arch/$(ARCH)/include	\
					$(PROJECT_DIR)/include	\
					$(PROJECT_DIR)/board/mach-$(CPU)	\
					$(PROJECT_DIR)/lib	\
					$(PROJECT_DIR)/lib/fatfs

ARCH_DIRS       :=  arch/$(ARCH)/
COMMON_DIRS     :=  common/
BOOT_DIRS       :=  boot/
BOARD_DIRS      :=  board/
PLATFORM_DIRS   :=  platform/
KERNEL_DIRS     :=  kernel/
EXT_LIB_DIRS    :=  lib/
ROOTFS_DIRS     :=  fs/
DRIVER_DIRS     :=  drivers/
INIT_DIRS       :=  example/ init/

OBJECT_EXEC		:=	$(OBJECT_PATH)/built-in.o
SOURCE_DIRS		:=	$(ARCH_DIRS) $(COMMON_DIRS) $(BOOT_DIRS) $(BOARD_DIRS) $(PLATFORM_DIRS)	\
					$(KERNEL_DIRS) $(EXT_LIB_DIRS) $(ROOTFS_DIRS) $(DRIVER_DIRS) $(INIT_DIRS)
INCLUDE_DIRS	:= 	$(patsubst %, -I %, $(INCLUDE_DIRS))

export ARCH TYPE CLASS CPU VENDOR CC CXX LD AR OBJDUMP OBJCOPY READELF
export LIBS_PATH LIBS EXTRA_FLAGS BUILD_CFLAGS MACROS CONF_MAKEFILE
export PROJECT_DIR LINK_SCRIPT DTC BUILD_SCRIPT INCLUDE_DIRS
export IMAGE_PATH OBJECT_PATH OBJECT_EXEC 
export TARGET_EXEC TARGET_BINY TARGET_IMGE TARGET_NASM TARGET_LASM TARGET_MMAP

obj-y			+=	$(SOURCE_DIRS)

VPATH			:= 	$(SOURCE_DIRS)
# *********************************************************************

force:
.PHONY:			all clean distclean config dtbs info debug

all : dtbs $(OBJECT_EXEC) force
	$(Q)$(MAKE) -C $(ARCH_DIRS) all

$(OBJECT_EXEC): force
	$(Q)$(MAKE) -C $(PROJECT_DIR) -f $(BUILD_SCRIPT) _build

clean:
	$(Q)$(MAKE) -C $(ARCH_DIRS) clean
	$(Q)$(MAKE) -f $(BUILD_SCRIPT) _clean
	rm -rf $(OBJECT_EXEC)
	
	$(Q)$(MAKE) -C $(OUTPUT_PATH)/dts clean

distclean:
	$(Q)$(MAKE) -C $(ARCH_DIRS) distclean
	$(Q)$(MAKE) -f $(BUILD_SCRIPT) _distclean
	$(Q)$(MAKE) -C $(OUTPUT_PATH)/dts distclean

dtbs:
	$(Q)$(MAKE) -C $(OUTPUT_PATH)/dts

config: $(CONF_HEADER) force
$(CONF_HEADER): $(CONF_MAKEFILE) force
	$(call create_header, $@, $<)

ifeq ($(CONFIGS),)
$(CONF_MAKEFILE):
else
$(CONF_MAKEFILE): force
	$(call create_conf, $@, $(CONF_GERNERIC))
endif

info:
	@echo "------------------------------------------------------------------------"
	@echo "                 project      : $(TARGET)"   
	@echo "                 arch         : $(ARCH)"
	@echo "                 arch type    : $(TYPE)"
	@echo "                 arch class   : $(CLASS)"
	@echo "                 vendor       : $(VENDOR)"
	@echo "                 cpu          : $(CPU)"
	@echo " "
	@echo "         ------------< ** information ** >------------ "
	@echo " "
	@echo "                 author       : Yang Yujun"
	@echo "                 e-mail       : <yujiantianhu@163.com>"
	@echo "------------------------------------------------------------------------"

debug: info
	$(Q)$(MAKE) -f $(BUILD_SCRIPT) _debug
	$(Q)$(MAKE) -C $(ARCH_DIRS) debug

# end of file
