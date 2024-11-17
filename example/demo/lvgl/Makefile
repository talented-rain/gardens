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
LVGL_DEMO_DIR	:=	$(shell pwd)

src-y	:=	$(shell find -L $(LVGL_DEMO_DIR) -name "*.c" -o -name "*.cpp")
obj-y   +=  $(patsubst %,%, $(src-y:.c=.o))

# end of file
