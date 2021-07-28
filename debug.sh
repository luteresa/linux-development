1.打开内核调试选项
Kernel hacking --->
Compile-time checks and compler options --->
	[*]Compile the kernel with debug info

2.启动虚拟机
qemu-system-arm   -nographic -M vexpress-a9 -m 1024M -kernel arch/arm/boot/zImage  -append "rdinit=/linuxrc console=ttyAMA0 loglevel=8" -dtb arch/arm/boot/dts/vexpress-v2p-ca9.dtb  -S  -s

-S:表示QEMU虚拟机会冻结CPU，知道远程的GDB输入相应控制命令
-s:表示在1234端口接收GDB的调试链接

3.gdb调试内核步骤
arm-none-eabi-gdb  --tui vmlinux
(gdb) target remote localhost:1234
b start_kernel
c
