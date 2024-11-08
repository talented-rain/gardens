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

dtb-y 			:=

include $(CONF_MAKEFILE)
include $(CPU)/Makefile

TARGET			:=	$(dtb-y)
DT_DIRS			:=	$(shell pwd)
DT_NAME			:=	$(DT_DIRS)/$(CPU)
DT_FILE			:=	$(DT_NAME)/$(TARGET).dts

INCLUDES		:=	$(DT_DIRS)/	\
					$(DT_DIRS)/$(CPU)/	\
					$(DT_DIRS)/dt-bindings/

DTC_INCLUDES	:= 	$(patsubst %, -i %, $(INCLUDES))
CXX_INCLUDES	:=	$(patsubst %, -I %, $(INCLUDES))

DTC				:=	$(PROJECT_DIR)/scripts/dtc/dtc
DTB				:=	$(DT_NAME)/$(TARGET).dtb
DTS				:=	$(DT_NAME)/.$(TARGET).dts
DEP				:=	$(DTS).d
TEMP			:=	$(DTS).temp

BUILD_FLAGS		:=	-nostdinc -undef -D__DTS__ -x assembler-with-cpp

all:
	cpp -Wp,-MD,$(DEP) $(BUILD_FLAGS) $(CXX_INCLUDES) -o $(TEMP) $(DT_FILE)
	$(DTC) -I dts -O dts $(DTC_INCLUDES) -o $(DTS) $(TEMP)
	$(DTC) -I dts -O dtb -o $(DTB) $(DTS)
	cp $(DTB) ../firmware.dtb

clean:
	rm -rf $(TEMP) 
	rm -rf $(DEP) 
	rm -rf $(DTS)

distclean:
	rm -rf ../firmware.dtb
	rm -rf $(DTB) 
	rm -rf $(TEMP) 
	rm -rf $(DEP) 
	rm -rf $(DTS)

check:
	$(DTC) -q -I dts -O dtb $(DTC_INCLUDES) $(DT_FILE)

version:
	$(DTC) -v

# end of file
