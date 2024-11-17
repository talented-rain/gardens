# HeavenFox 系统内核
# 

[English](./README.md) | **中文**

## 说明
    HeavenFox OS意在使用一种简化的方式来重写linux, 在整体框架上接近于linux内核, 而核心偏向于rtos.
    本项目多处重要代码的实现方式参考自linux内核, 但又不尽相同; 适合爱好者开发和探索. 也因如此, HeavenFox
    必然遵循GPL协议, 不论用于哪一种场合, 都是开源且自由的.

## 信息 (如有需要可邮件联系)
    作者:       杨雨俊 (Yang Yujun)
    创建时间:   2023.12.21
    电子邮箱:   <yujiantianhu@163.com>
    git仓库:    git@github.com:yujiantianhu/HeavenFox.git

## 功能描述
>>  支持多线程调度机制, 包括线程切换、时间片轮转、优先级抢占、睡眠与唤醒;
>>  支持隐式初始化, 允许灵活加入或删除驱动组件;
>>  支持字符设备驱动架构;
>>  支持framebuffer驱动架构及显示相关API (支持描点、字库读取、图片解析等);
>>  支持中断号虚拟化, 即所有硬件中断号均转换为唯一的虚拟中断号; 提供中断服务申请和释放操作;
>>  支持定时器中断链表, 允许定时事件自由注册和脱离;
>>  支持SD卡 (移植Fatfs), 并提供通用文件函数接口, 如: file_open/file_close, ...;
>>  支持设备树, 可从SD卡读取dtb文件并解析为device_node和platform_device;
>>  支持总线-设备-驱动匹配机制, 设备可人为注册, 或来自于设备树; 通过平台总线与驱动程序匹配;
>>  支持虚拟文件操作 (用于字符设备驱动), "打开"设备文件后将返回文件描述符fd, 由virt_open/virt_read/...等API操作字符设备;
>>  支持各级子目录及子文件自由选择是否参与编译, 由"obj-y"链接各源文件和目录;
>>  支持cpu引脚在设备树中以pinctrl形式定义, 内核提供pinctrl和gpio通用接口进行解析;
>>  支持时钟子系统、gpio中断接口;
>>  支持i2c总线架构: adapter - client - driver
>>  为多线程提供临界资源互斥机制: 等待队列、互斥锁、自旋锁、信号量;
>>  支持线程间通信机制: 邮箱消息;
>>  默认提供内核线程: 工作队列, 支持自由增删工作事件, 由内核线程轮询调用;
>>  内嵌dtc编译器, 可通过"make dtbs"或"make all"直接编译设备树文件 (生成.dtb);
>>  提供驱动程序: 触摸屏tsc2007驱动、环境传感器ap3216c、eeprom芯片at24c02等;
>>  已支持的cpu: imx6ull, xc7z010;
>>  支持内核配置自由化, 可通过创建和编写"configs/mach/xxxx_defconfig", 自动生成auto.conf and autoconf.h;
>>  支持"lib/"路径下第三方项目源码单独编译, 生成静态库文件并链接到内核镜像 
    
## 使用方法
    make all        编译整个工程;
    make            自动调用make all;
    make dtbs       单独编译设备树;
    make clean      清空除镜像文件、静态库以外的中间文件 (如.o, .d等);
    make distclean  在make clean基础上, 删除镜像、dtb和静态库;
    make config     生成auto.conf和autoconf.h, 也可以使用make CONFIGS=xxxx_defconfig config直接更新配置
    make info       获取工程信息;
    make libs       单独编译第三方库源码;
    make local      仅编译内核代码, 设备树和第三方库不编译
    
    也可以使用cmake (不建议, CMakeLists.txt可能非最新):
    mkdir -p ./build
    cd ./build
    make distclean
    cmake -DCMAKE_BUILD_TYPE=debug ..
    make
    
    镜像路径:   ./boot/image/HeavenFox.img
    设备树路径: ./boot/image/firmware.dtb
    
## 文件夹说明
    boot/:          包含设备树、最终生成的镜像文件, 以及内核启动相关的代码;
    configs/:       可编写的功能配置文件路径;
    build/:         CMake编译生成的中间文件存放处;
    document/:      文档相关;
    scripts/:       工具或通用脚本, 如dtc编译器, jlink scripts, ..., 以及Makefile.build;
    objects/:       编译生成的中间文件, 将存放于此;
    
    arch/:          包括芯片启动、异常向量表、寄存器/协处理器读写等与CPU架构高度相关的文件;
    board/:         包括外设初始化、CPU通用SDK等单板相关代码;
    common/:        公共、通用的代码, 是本项目最基础的部分;
    drivers/:       驱动程序, 包括CPU自身的资源驱动、单板的资源驱动等;
    example/:       例程;
    fs/:            文件系统相关;
    include/:       头文件统一存放处;
    init/:          内核初始化相关代码, 如main.c;
    kernel/:        内核调度器及多线程相关代码;
    lib/:           第三方库源码;
    platform/:      平台架构相关, 是除了common文件夹之外的第二大基础代码, 提供更多功能的函数接口
