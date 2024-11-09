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

# 'all' must be the first target
all:

include $(CONF_MAKEFILE)

obj-y		+=	cpu/
obj-y		+=	kernel/
obj-y		+=	lib/

vector-y	:= 	lib/vectors.o
start-y		:=	cpu/$(TYPE)/start.o
head-y		:=	kernel/head.o

# get current path
arch_path	:= 	$(shell pwd)
arch_output	:=	$(patsubst $(PROJECT_DIR)%, $(OBJECT_PATH)%, $(arch_path))

head-objs	:=	$(vector-y) $(start-y) $(head-y)
depd-objs	:=	$(foreach file, $(head-objs), $(addsuffix .d, $(file)))

head-objs	:=	$(addprefix $(arch_path)/, $(head-objs))
head-objs	:=	$(patsubst $(PROJECT_DIR)%, $(OBJECT_PATH)%, $(head-objs))

ifneq ($(depd-objs),)
depd-objs	:=	$(addprefix $(arch_path)/, $(depd-objs))
depd-objs	:=	$(patsubst $(PROJECT_DIR)%, $(OBJECT_PATH)%, $(depd-objs))
depd-objs	:=	$(wildcard $(depd-objs))

include $(depd-objs)
endif

define arch_build
	if [ ! -d $(arch_output) ]; then mkdir -p $(arch_output); fi
	$(1) -Wp,-MD,$@.d -c $(BUILD_CFLAGS) $(MACROS) $(INCLUDE_DIRS) $(EXTRA_FLAGS) -o $(2) $(3)
endef

force:
.PHONY: all clean distclean

all: $(TARGET_BINY) force
$(TARGET_BINY) : $(TARGET_EXEC) force
	$(OBJCOPY) -O binary -S $(TARGET_EXEC) $@
	$(OBJDUMP) -D -m $(ARCH) $(TARGET_EXEC) > $(TARGET_NASM)
	$(OBJDUMP) -D -m $(ARCH) -S -l $(TARGET_EXEC) > $(TARGET_LASM)

$(TARGET_EXEC) : $(head-objs) force
	$(CC) -T$(LINK_SCRIPT) $(BUILD_CFLAGS) $(EXTRA_FLAGS) -Wl,-Map=$(TARGET_MMAP) -o $@ $(head-objs) $(OBJECT_EXEC) $(LIBS_PATH) $(LIBS)

$(arch_output)/%.o : %.S force
	$(call arch_build, $(CC), $@, $<)

$(arch_output)/%.o : %.c force
	$(call arch_build, $(CC), $@, $<)

$(arch_output)/%.o : %.cpp force
	$(call arch_build, $(CXX), $@, $<)

clean:
	rm -rf $(head-objs) $(depd-objs)

distclean:
	rm -rf $(head-objs) $(depd-objs)
	rm -rf $(TARGET_EXEC) $(TARGET_BINY) $(TARGET_IMGE) 
	rm -rf $(TARGET_NASM) $(TARGET_LASM) $(TARGET_MMAP)

debug:
	@echo '==========>' $(head-objs)

# end of file
