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

LWIP_DIRS	:=	$(shell pwd)
src-y		+=	$(shell find -L $(LWIP_DIRS) -type f -name "*.c" -o -name "*.cpp" -o -name "*.S")

inc-y		+=	-I $(LWIP_DIRS)/src/include
inc-y		+=	-I $(LWIP_DIRS)/port

VPATH		+=	$(sort $(dir $(src-y)))

# end of file
