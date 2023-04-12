# Code Generation Options

## -fpic

Generate position-independent code (PIC) suitable for use in a shared library, if supported for the target machine. Such code accesses all constant addresses through a global offset table (GOT). The dynamic loader resolves the GOT entries when the program starts (the dynamic loader is not part of GCC; it is part of the operating system). If the GOT size for the linked executable exceeds a machine-specific maximum size, you get an error message from the linker indicating that -fpic does not work; in that case, recompile with -fPIC instead. (These maximums are 8k on the SPARC, 28k on AArch64 and 32k on the m68k and RS/6000. The x86 has no such limit.)

Position-independent code requires special support, and therefore works only on certain machines. For the x86, GCC supports PIC for System V but not for the Sun 386i. Code generated for the IBM RS/6000 is always position-independent.

When this flag is set, the macros \_\_pic\_\_ and \_\_PIC\_\_ are defined to 1.

如果目标机器支持，则生成适用于共享库的位置无关代码（PIC）。这种代码通过全局偏移表（GOT）访问所有常量地址。当程序启动时，动态加载器解析GOT条目（动态加载器不是GCC的一部分，而是操作系统的一部分）。如果链接可执行文件的GOT大小超过机器特定的最大大小，则链接器会显示错误消息，指示-fpic不起作用；在这种情况下，请改用-fPIC重新编译。（这些最大值为SPARC上的8k，AArch64上的28k，m68k和RS/6000上的32k。x86没有这样的限制。）

位置无关代码需要特殊支持，因此只适用于某些机器。对于x86，GCC支持System V的PIC，但不支持Sun 386i的PIC。生成的IBM RS/6000代码始终是位置无关的。

设置此标志时，宏__pic__和__PIC__将被定义为1。

## -fPIC

If supported for the target machine, emit position-independent code, suitable for dynamic linking and avoiding any limit on the size of the global offset table. This option makes a difference on AArch64, m68k, PowerPC and SPARC.

Position-independent code requires special support, and therefore works only on certain machines.

When this flag is set, the macros \_\_pic\_\_ and \_\_PIC\_\_ are defined to 2.

如果目标机器支持，则发出适用于动态链接的位置无关代码，并避免对全局偏移表大小的任何限制。此选项在AArch64、m68k、PowerPC和SPARC上有所不同。

位置无关代码需要特殊支持，因此只适用于某些机器。

设置此标志时，宏__pic__和__PIC__将被定义为2。
