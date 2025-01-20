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
LWIP_DEMO_DIR	:=	$(shell pwd)

src-y	:=	$(shell find -L $(LWIP_DEMO_DIR) -name "*.c" -o -name "*.cpp")
obj-y   +=  $(patsubst %.c,%.o, $(filter %.c, $(src-y)))
obj-y   +=  $(patsubst %.cpp,%.o, $(filter %.cpp, $(src-y)))

# end of file
