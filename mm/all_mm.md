目录

[](#_Toc48641031)

>   [](#_Toc48641032)

>   [](#_Toc48641033)

>   [](#_Toc48641034)

>   [](#_Toc48641035)

>   [](#_Toc48641036)

[](#_Toc48641037)

>   [](#_Toc48641038)

>   [](#_Toc48641039)

>   [](#_Toc48641040)

>   [](#_Toc48641041)

[](#_Toc48641042)

>   [](#_Toc48641043)

>   [](#_Toc48641044)

>   [](#_Toc48641045)

>   [](#_Toc48641046)

[](#_Toc48641047)

>   [](#_Toc48641048)

>   [](#_Toc48641049)

>   [](#_Toc48641050)

>   [](#_Toc48641051)

>   [](#_Toc48641052)

>   [](#_Toc48641053)

[](#_Toc48641054)

>   [](#_Toc48641055)

>   [](#_Toc48641056)

>   [](#_Toc48641057)

>   [](#_Toc48641058)

>   [](#_Toc48641059)

# 1 硬件原理和分页管理

## 1.1 CPU寻址内存，虚拟地址、物理地址

**(1)寻址内存：**

CPU访问外设，有两种类型，一个是内存空间，一个是IO空间;

**IO空间**，X86通过in/out指令访问外设，IO空间只存在X86架构，在RISC架构不存在；

**内存空间**，CPU通过指针访问所有内存空间，内存空间分为两类，**普通内存**和**位于内存空间的寄存器**。其他设备的寄存器，比如通过I2C总线访问触摸屏的寄存器，与CPU内存空间无关。

![](media/50a48e80431ef2346c44e9ee7ad0bd92.jpg)

**(2) MMU原理**

对于一个支持MMU的CPU，只要开启MMU，CPU跟程序员视角一致，看到的永远是虚拟地址；

在访问内存空间时，CPU发一个虚拟地址(指针)，MMU把虚拟地址映射为物理地址。

其他硬件设备，比如DMA/IVE(硬件模块不带mmu)访问也是物理地址；

**(3)虚拟地址和物理地址：**

比如在两个不同的进程QQ/WECHAT里，可以分别定义一个变量，虚拟地址相同(都为1G)；真正访问的时候，经过MMU映射到实际不同的物理地址。

![](media/89a67e50fdd3e5cb6d8f3912d1cfc7ca.jpg)

每个进程都维护自己的页表，相同的虚拟地址可以映射到不同的物理地址；

当CPU访问0x1234560时，先查询页地址0x1234对应的物理地址(比如1M)，560为页内偏移

patch

int \*p=1m; //对指针赋值物理地址，是错误的

**(4) 物理地址的本质**

在Linux 内核，物理地址定义为一个无符号64/32位整数；

32处理器虚拟地址最大4G，物理地址不一定32位，可以大于4G；

物理地址位数可以大于虚拟地址位数；

## 2.MMU以及RWX权限、kernel和user模式权限

32位处理器，页表有32位，但实际维护页表只用到高20位，低12位用来存放其他信息比如权限

![](media/59422b10736fb696db9d3c5aee37e7d5.jpg)

CPU访问一个地址，不仅要做虚拟/物理地址页表地址查询，还要检查页表权限，出现非法操作时，MMU拦截CPU访问请求，并报page
fault，CPU收到MMUpage
fault中断时，报段错误，发信号11，进程默认信号处理方式是挂掉。

![](media/442ba2ae3413cf8938acb0ce05ccd754.png)

找不到物理地址，或者权限不对，发生段错误；

gdb layout src

meltdown漏洞，利用时间旁路攻击原理，从用户空间获取内核空间数据；

## 3.内存的zone: DMA、Normal和HIGHMEM

32位Linux内核空间物理地址映射在3G～4G，分三个区域，

ZONE_DMA(0～16M):DMA内存分配区；

ZONE_NORMAL(16MB～896MB): 普通映射的内存区域；

ZONE_HIGHMEM(896MB\~):高端内存区域，其中的页不能永久映射到内核地址空间；内核一般不使用，如果要使用，通过kmap做动态映射；

存在高端内存的原因是，当物理内存大于虚拟内存寻址范围(1G)时，需要做**非线性映射**，才能在访问所有物理内存空间范围。32系统一般都划分896M以上物理地址为高端内存。

在目前流行的64位CPU中，由于虚拟寻址位数足够，就不存在高端内存概念了。

![](media/aaec31c0f8b0f0bea017eb64d643ac62.png)

896M以下，开机直接映射好(通过virt_to_phys/phys_to_virt简单线性映射)，896MB以上动态分配，可以被内核和用户空间访问；

对于64位CPU有足够寻址能力，就不存在869MB问题了，64位处理器，一般没有HGIHMEM
ZONE；；

对于一个嵌入式设备，内存太小，也不存在 HIGHMEM区域；

**DMAzone存在的根本原因是DMA硬件有缺陷**，比如一个典型32位系统ISA设备的DMA只有24位寻址能力，也就是只能访问内存的前16M内存，而整个系统的最大物理内存寻址甚至可以大于1G，超过内核的虚拟空间；

DMA没有MMU，所以只能访问连续物理内存；少数最新的DMA具有MMU，也可以访问物理不连续的内存，MMU页表映射即可。

当为一个DMA有缺陷设备申请内存时，传入标记GFP_DMA，在DMA
ZONE区域分配内存，如果DMA可以访问所有内存空间，则不用受限于DMA_ZONE。而其他普通设备，也可以申请DMA_ZONE.

申请DMA内存，会填入设备访问范围，根据范围确定申请的内存区域；

DMA内存不一定来自于DMA ZONE，DMA ZONE也不一定用于DMA

如果系统所有DMA都没有缺陷，则不存在dma_zone；

DMA直接访问内存，不会让外设访问速度更快，DMA最大好处是解放CPU资源，速度受限外设总线及访问频率；

## 4.Linux内存管理Buddy算法

**Buddy 算法直面物理内存；**

Buddy算法把内存中的所有页面按照2的幂次进行分块管理，分配的时候如果没有相应大小块，就把大的块二分成小块；释放的时候，回收的块跟相邻的空闲伙伴块又能合并成大块；

BUDDY页面的分配和使用情况可以通过proc接口/proc/buddyinfo来查看；

![](media/502f8458f4c31e1c7b431a9d6adee240.png)

Buddy有个缺陷，会造成许多内存碎片，比如总和还剩余很大，但是没有足够连续的空余内存可用。

早期采用预留分配方法，给予显卡，摄像头等预留内存，即使设备不使用，也会预留。

## 5.连续内存分配器(CMA)

![](media/f0cc2935990fe273c049ad91edd5fd08.png)

分配专门的CMA区域，用于DMA设备的内存申请，比如摄像头，

当摄像头设备不用时，该区域可以用于其他内存分配；

当使用摄像头时，将该区域内的所有已申请内存，拷贝到其他分散的页，并且修改对应页表。

![](media/320ba2181f44e51c8278389a14fdcd07.png)

这样CMA大块连续内存，就不会被浪费掉，也保持了大块连续内存的访问需求；

把不同的DMA设备，放在不同的CMA，就不会导致内存碎片化；

《DMA若干误解的彻底澄清》；

Doccumentation/devicetree/binding/reserved-memory/reserved-meory.txt

# 2:内存的动态申请和释放

## 1.slab、kmalloc/kfree、/proc/slabinfo和slabtop

Buddy
是直面物理内存的，所有的内存分配，最终都通过Buddy的get_free_page/page_alloc分配；

Buddy的粒度太大，最小分配一页(4k); 而我们常常需要分配小内存；

所以Linux引入一个**二级分配**的概念：

1.内核分配内存，调用kmalloc()/kfree()--调用slab--再调用Buddy ;

2.用户空间malloc/free--调用C库--C库通过brk/mmap调用Buddy；

free释放的内存，只是释放给C库，未必真正释放；释放给C库的内存，其他进程无法使用，共享代码段，数据段独立；

![](media/d6589cc9b8d5787afe1e294718e6aa9a.jpg)

**(1)slab原理：**

就是从Buddy拿到一个/多个页，分成多个相同大小的块，比如进程控制块task_struct这种内核中常用的结构体，可以先从Buddy申请一个slab池，实际kmalloc分配的时候，直接从slab里分配。可以提高速度，也可以使内存得到充分使用，减少内存碎片；

每一个slab里的所有块，大小一定是相同的；

sudo cat /proc/slabinfo

![](media/6144c4436689897672f92d235414ebd3.png)

cat /proc/meminfo

![](media/a9b557f33ca99abfb767b2bba2cf710d.png)

slab也是一个内存泄漏的源头；

应用程序free内存，未必一定释放，可以通过mallopt设置free内存的触发阈值，触发阈值，free才真正在Buddy
释放；

(2) kmalloc / vmalloc详细过程

kmalloc从低端内存区域分配，该区域是开机就一次性映射好；所以kmalloc申请，不需要做映射,
且在物理内存上是连续的。

phys_to_virt/virt_to_phys只是一个简单的内存线性偏移。

vmalloc申请的虚拟内存，可以从普通物理内存空间分配，也可以从低端内存分配；

调用vmalloc申请，会有一个虚拟地址到物理地址映射过程。

ioremap映射的也是vmalloc虚拟地址，不过不用申请内存，ioremap映射的是寄存器；

![](media/14f91e046e015f8cce0f911ccb27953c.png)

映射跟分配是两回事，低端被映射之后，不一定被使用；

sudo cat /proc/vmallocinfo \|grep ioremap
//可以查看虚拟地址映射物理地址/寄存器关系

![](media/a956191954ae0fff88aadd2a06a6a5b7.png)

**kmalloc和vmalloc区别**大概可总结为：

1vmalloc分配的一般为普通内存，只有当内存不够的时候才分配低端内存；kmalloc从低端内存分配。

2vmalloc分配的物理地址一般不连续，而kmalloc分配的物理地址连续，两者分配的虚拟地址都是连续的；

3vmalloc分配的一般为大块内存，而kmallc一般分配的为小块内存；

## 2.用户空间malloc/free与内核之间的关系

问题1：malloc:VSS , RSS

p = malloc(100M);//分配过程

1.在进程的堆中找到一个空闲地址，比如1G，创建一个VMA(virtual memory
area),权限可读写;

2.将p=1G\~1G+100M全部映射到零页(标记为只读)；

3.当读p虚拟内存的时候，全部返回0，实际上任何内存都未分配;

4.当写内存时，比如写1G+30M处，而该地址映射向零页(只读)，MMU发现权限错误，报page
fault，CPU可以从page
fault中得到写地址和权限，检查vma中的权限，如果vma标明可写，那么触发缺页中断，内核分配一个4K物理页，重新填写1G+30M页表，使其映射到新分配的物理页；

这样该页就分配了实际的物理内存，其他所有未使用到的虚拟地址亦然映射到零页。

如果检查vma发现没有可写权限，则报段错误；

如果检查vma合法，但是系统内存已经不够用，则报out of memory(OOM).

在真正写的时候，才去分配，这是按需分配，或者惰性分配；

![](media/76056b4cf5a5ac418057cb8ac36d185c.jpg)

只申请了VMA，未实际拿到物理内存，此时叫VSS；拿到实际内存后是RSS(驻留内存)；

![](media/b1afb692a76f8950bf78d3c2cca5934d.png)

属于按需分配demanding page，或者惰性分配lazy allocation；

对于代码段(实际读时，才实际去分配内存，把代码从硬盘读到内存)，数据段都是类似处理，实际使用时，才会实际分配内存；

## 3内存耗尽(OOM)、oom_score和oom_adj

在实际分配内存，发现物理内存不够用时，内核报OOM，杀掉最该死进程(根据oom_score打分)，释放内存；

打分机制，主要看消耗内存多少(先杀大户)，mm/oom_kill.c的badness()给每个进程一个oom_score，一般取决于：

驻留内存、pagetable和swap的使用；

oom_score_adj:oom_score会加上oom_score_adj这个值；

oom_adj：-17\~15的系数调整

关掉交换内存分区:

sudo swapoff -a

sudo sh -c ‘echo 1 \> /proc/sys/vm/overconmit_memory’

git grep overcommit_memory

dmesg //查看oom进程的oom_score

查看chrome进程的oom_score

pidof chrome

cd /proc/28508/

cat oom_score

d2_mm3.jpg

![](media/60320ab44bb614699daf3e9d8987662f.png)

由上图可知，

1将oom_adj值设置越大，oom_score越大，进程越容易被杀掉；

2.将oom_adj设置更大时，普通权限就可以；要将oom_adj设置更小，需要root权限；

即设置自身更容易死掉，自我牺牲是很容易的，设置自己不容易死，是索取，需要超级权限才可以；

## 4Android进程生命周期与OOM

android进程的生命周期靠OOM来驱动；

android手机多个进程间切换时，会动态设置相应oom_adj，调低前台进程的oom_adj，调高后台进程的oom_adj，当内存不够时，优先杀后台进程。所以内存足够大，更容易平滑切换。

对于简单的嵌入式系统，可以设置当oom时，是否panic

cd /proc/sys/vm

cat panic_on_oom

mm/oom_kill.c oom_badness()函数不停调整进程oom_score值；

总结一个典型的ARM32位，Linux系统，内存分布简图；

![](media/af35bc71b0929f7331e05f24c4465db6.png)

ARM32位内核空间3G-16M～4G

3G-16M～3G-2M用来映射KO

3G-2M\~3G:可以用kmap申请高端内存，用kmap，建立一个映射，临时访问页，访问完后kunmap掉；

进程的写时拷贝技术，mm/memory.c/cow_user_page用到kmap映射；

作业

1.看/proc/slabinfo，运行slabtop

运行mallopt.c程序：mallopt等

运行一个很耗费内存的程序，观察oom memory

通过oom_adj调整firefox的oom_score

# 3.进程的内存消耗和泄漏

## 1.进程的VMA

(1)在Linux系统中，每个进程都有自己的虚拟内存空间0～3G；

内核空间只有一个3G～4G；

![](media/39176a76208129a24056fa3f9bf23e04.jpg)

进程通过系统API调用，在内核空间申请内存，不统计在任何用户进程；进程消耗内存，单指用户空间内存消耗；

(2)
LINUX用task_struct来描述进程，其中的mm_struct是描述内存的结构体，mm_struct有一个vma列表，管理当前进程的所有vma段。

![](media/c0e4305d1d352443eb995b58eaf1e6b1.png)

每个进程的内存由多个vma段组成：

(3)查看VMA方法：

**1.pmap**

![](media/13ccc825d7e11b6e9345534002a6bf25.png)

由图知，从接近0地址开始，第一个4K是只读代码段，第二个4K是只读数据段，还有其他共享库代码段，堆栈等；

可见一个进程的VMA涵盖多个地址区域**，但并没有覆盖所有地址空间**。VMA未覆盖的地址空间是illegal的，访问这些地址，缺页中断，发生pagefault.

**2.cat /proc/pid/maps**

![](media/200b79c79bbed7cc3bf79b82455a09c5.png)

读文件形式，与pmap一一对应；

**3.cat /proc/pid/smaps**

更详细的描述

00400000-00401000 r-xp 00000000 08:15 20316320 /home/leon/work/linux/mm/a.out

Size:         4 kB

KernelPageSize:    4 kB

MMUPageSize:      4 kB

Rss:          4 kB

Pss:          4 kB

Shared_Clean:     0 kB

Shared_Dirty:     0 kB

Private_Clean:     4 kB

Private_Dirty:     0 kB

Referenced:      4 kB

Anonymous:       0 kB

LazyFree:       0 kB

AnonHugePages:     0 kB

ShmemPmdMapped:    0 kB

Shared_Hugetlb:    0 kB

Private_Hugetlb:    0 kB

Swap:         0 kB

SwapPss:        0 kB

Locked:        0 kB

VmFlags: rd ex mr mw me dw sd

查看VMA的三个方法对比

![](media/fbe52107869af6ec71998a87b12c39c2.png)

VMA的来源，代码段，数据段，堆栈段。

![](media/1d4cbcb84b83d8eb509049e88dfcda9f.png)

**VMA是linux最核心数据结构之一。**

## 2.page fault的几种可能性，major和minor

![](media/b7e5bf2c1ad0ec776d99dfc99c90528d.png)

mmu给cpu发page
fault时，可以从寄存器读到两个元素，**pagefault地址**，**pagefault原因**。

(1)访问Heap堆(首次申请，不是从Libc获取)，第一次写，发生pagefault，linux检查VMA权限，发现权限合法，发缺页中断，申请一页内存，并更新页表。

(2)访问空区域，访问非法，发段错误；

(3)访问代码段, 在此区域写，报pagefault，检查权限发现错误，报段错误；

(4)访问代码段,在此区域读/执行，linux检查权限合法，若代码不在内存，那么申请内存页，把代码从硬盘读到内存。

伴随I/O的pagefault, 叫major pagefault, 否则minor pagefault.

major pagefault耗时远大于 minor pagefault.

\\time -v python hello.py

**3.内存是如何被瓜分的:** ：vss、rss、pss和uss

rss是不是代表进程的内存消耗呢，NO。

![](media/6073a13855c3d6c6f0334b953cd7f285.png)

**VSS**：单个进程全部可访问的地址空间，但未必所有虚拟地址都已经映射物理内存；

**RSS**：驻留内存，单个进程实际占用的物理内存大小(不十分准确的描述)；上图的进程1

**PSS**:
比例化的内存消耗，相对RSS，将共享内存空间按被共享进程数量比例化；上图的C库4被三个进程共享，所以4/3;

**USS**：进程独占内存，比如上图的堆6。

案例，连续运行两次a.out，查看内存占用情况

运行程序a.out

./a.out &

pidof a.out

cat /proc/pid/smaps \|more

![](media/5280f46620090ef6c3fa837ef5fbdfbe.png)

./a.out &

pidof a.out

![](media/552b45603d10618e2d7b0809e91253b3.png)

cat /proc/pid_2/smaps \|more

PSS变化

shared_clean

private_clean

## 4.应用内存泄漏的界定方法

![](media/5835a1555e4cb021132a1d4826cc318c.png)

统计系统的内存消耗，查看PSS。

检查有没有内存泄漏，检查USS

./smem

smem –pie=command

![](media/988c1868230625fb4282deb0ff7e70c7.png)

smem –bar=command

![](media/64ac61c220baae25eb3ee449ad3bbc89.png)

![](media/40574a9d8e29e2f10c507ed96a5c1b4e.png)

android里面有类似的工具，procmem/procrank

smem分析系统内存使用是通过/proc/smaps的，procrank是通过分析/proc/kpagemap。

## 5.应用内存泄漏的检测方法：valgrind和addresssanitizer

其他查询内存泄漏工具：

valgrind:

	gcc -g leak-example.c -o leak-example

	valgrind --tool=memcheck --leak-check=yes ./leak-example

![](media/8323ab1eb838a435ddc6f82f731969ed.png)

==20468== HEAP SUMMARY:

==20468==   in use at exit: 270,336 bytes in 22 blocks

==20468==  total heap usage: 44 allocs, 22 frees, 292,864 bytes allocated

==20468==

==20468== 258,048 bytes in 21 blocks are definitely lost in loss record 2 of 2

==20468==  at 0x4C2DB8F: malloc (in
/usr/lib/valgrind/vgpreload_memcheck-amd64-linux.so)

==20468==  by 0x4005C7: main (leak-example.c:6)

![](media/06e725b77df25e31fdb09166e84dd4e1.png)

valgrid是在虚拟机跑APP，速度很慢；

**新版gcc4.9以后集成了asan**

asan:

	gcc -g -fsanitize=address ./leak-example.c

	gcc -fuse-ld=gold -fsanitize=address -g ./lsan.c

![](media/79984ebb28cbf21cd21690b20aa13ddd.png)

\#1 0x40084e in main lsan.c:9

内存泄漏不一定在用户空间，排查内核空间，检测slab , vmalloc

slaptop, 检查申请和释放不成对。

在内核编译，打开kmemleak选项。

**5.工程调试内存泄漏问题一般步骤：**

1.  meminfo, free 多点采样确认有内存泄漏。

2.定位,smem检查用户空间

![](media/66e0b0b993fdacac3ac33583aec70b62.png)

USS在不断增加。

3.slab,检查内核空间。

cat /proc/slabinfo

其他查看内存信息的方法

cat /proc/meminfo

cat /proc/buddyinfo

cat /proc/zoneinfo

cat /proc/meminfo

# 4.**内存与I/O的交换**

## **1. page cache**

## [media/image36.png](media/image36.png)

~   C:\\Users\\tvt\\AppData\\Local\\Temp\\企业微信截图_15985003772210.png

**Linux读写文件过程；**

**read**:用户进程调用read命令，内核查询读取的文件内容是否在内存(内核page
cache)中，若该页内容缓存在内存中，直接读取返回给用户进程；若缓存不存在，则启动BIO，从硬盘读取该页面到内存，再送给用户进程；

**write**过程：比如往某文件5K处写入10byte，内核先查询该页是否在内存缓存(内核page
cache)，不在，同read,从硬盘读取该页4\~8K到内存，再往5k处写入10byte,标明该页为脏页；

写回磁盘时机，则由内存管理的BIO机制决定。

Linux读写文件两种方式：read/write,mmap

**相同点**：都经过page cache中介操作磁盘文件；

**区别**：

read/write：有一个用户空间和内核空间的拷贝过程；

mmap：指针直接操作文件映射的虚拟地址，省略了用户空间和内核空间的拷贝过程。

缺点是，很多设备不支持mmap，比如串口，键盘等都不支持。

## **2.free命令的详细解释**

![](media/d7c58c6669abe9c2ae39efcd468d04b8.png)

**total:系统总的内存**

**第一行used:所有用掉的内存，包括app,kernel,page cached(可回收)**

**第一行free:=total-used**

**第一行buffers/cached：page cached内存**

**第二行free = 第一行free+buffers+cached**

**114380=31040+8988+74352**

**第二行used = 第一行used-buffers-cached**

**40072=123412-8988-74352**

**第一行used是从buddy的角度，统计所有用掉的内存(包括page cached)；**

**第二行uesed，是从进程角度，包括app/kernel/ , 不含page cached(可回收)。**

**cached/buffer区别：**

**cached:通过文件系统访问；**

**buffer:裸地址直接访问，存放文件系统的元数据(组织数据的数据)；**

![](media/00b095ccc61826215ebdd013573e4af7.jpg)

**新版free，去掉了buffer/cached的区别，加入一个available，预估可用内存**

![](media/c67661d0f87773fc26d75b7801818d4d.png)

**在cat /proc/meminfo也可以看到预估项MemAvailable**

![](media/823575961fbdd036cdc41215e2b66bd6.png)

**查看实现方法git grep MemAvailable**

![](media/f0b344afe4225111840d2267f7ed17f7.png)

**vim  fs/proc/meminfo.c  **

**搜MemAvailable，实现函数si_mem_available**

**git grep  si_mem_available	**

**vim mm/page_alloc.c-- \>long si_mem_available(void)**

## **3.read、write和mmap**

**mmap映射文件过程，实际上在进程创建一个vma，映射到文件，当真正读/写文件时，分配内存，同步磁盘文件内存。**

![C:\\Users\\tvt\\AppData\\Local\\Temp\\企业微信截图_15985006812726.png](media/f111ffad9f0b57fb6066ba2434cf76c8.png)

**其本质是虚拟地址映射到page cached, page cached再跟文件系统对应。**

![](media/055d32a4fa1c58e3fdee1e14c73a4033.png)

**这里的page cached是可以回收的。**

## **4.file-backed的页面和匿名页**

**在一个elf文件中，代码段的本质，就是对应page cached；**

**真正执行到的代码，才会从磁盘读入内存，并且还可能被踢走，下次再执行，可能需要重新从磁盘读取。**

![](media/da5bba8fb4fd605f916d4077f34ea1fa.jpg)

**Reclaim可回收的**

**有文件背景的页面，叫file-back page, 可回收；**

**匿名页：anon， 不可回收，常驻内存；**

**pmap pid**

**Anon:  stack、heap、 CoW pages**

**File-backed：代码段，映射文件，可回收。**

## **5.swap以及zRAM**

![](media/c46a8794bb6c17c59394f6961a97ef26.png)

**案例，当同时运行word和qq，word需要400M匿名页，qq需要300M匿名页，而物理内存只有512M，如何运行呢，此时伪造一个可swap的文件，供anon匿名页交换。**

![](media/4b6061318947781521a8e2f3c9c92165.jpg)

**在内核配置CONFIG_SWAP，支持匿名页swap，不配置，普通文件swap依然支持；**

**SWAP分区，对应windows的虚拟内存文件pagefile.system。**

## **6.页面回收和LRU**

**局部性原理：最近活跃的就是将来活跃的，最近不活跃的，以后也不活跃。**

**包括时间局部性，空间局部性。**

LRU：Least Recently Used最近最少使用，是一种常用的页面置换算法，

![](media/60e4e7ab194854481a886ec57e98589a.png)

**真实统计过程，无关进程，只统计页面的活跃度；**

**案例，开启浏览器chrome；长期不使用，运行oom.c消耗掉系统所有内存，再去第一次打开网页时，速度会很慢，因为需要重新加载被踢出去的不活跃页面。**

**sudo swapoff –a**

**echo 1 \> /proc/sys/vm/overcommit_memory**

**cat /proc/pid/smps \>1**

**./oom.out**

**cat /proc/pid/smaps \>2**

**meld 1 2**

**可见命中的部分page cached，被LRU踢走。**

**CPU\>\>mm\>\>io**

**嵌入式设备，一般不用swap，不使能swap，因为：**

**1.嵌入式磁盘速度很慢；**

**2.FLASH读写寿命有限；**

**zRAM**

在物理内存划分一部分，作为“swap”分区，用来做页面回收置换，利用强大CPU算力换取更大内存空间；

Iph5.jpg

![](media/8a097c414dddf41eef2a0ba53ea6539e.jpg)

内核使能swap

echo \$((48\*1024\*1024)) \> /sys/block/zram0/disksize

打开swap分区：

swapon –p 10 /dev/zram0

cat /proc/swaps

swapoff –a 不能关掉文件背景页面。

不带pagecached的IO， direct IO。 在用户态根据业务特点做cached.

类似于CPU跳过cache，直接访问mem;

# 5 其他工程问题以及调优

## 1.DMA和cache一致性

![C:\\Users\\tvt\\AppData\\Local\\Temp\\企业微信截图_15985175222511.png](media/fdc2be85ab4d11efa4dcf767bd8d4633.png)

1.不带CACHE

自己写驱动，申请DMA，可以用Coherent DMA buffers

void \* dma_alloc_coherent(struct device \*dev, size_t size, dma_addr_t
\*dma_handle,gfp_t flag);

void  dma_free_coherent (struct device \*dev, size_t size,void \*cpu_addr,
dma_addr_t dma_handle);

CMA和此API自动融合，调用dma_alloc_coherent()将从CMA获得内存，不带cache。

2.流式DMA:

操作其他进程的地址，比如tcp/ip报文的缓存，无法控制DMA地址源，

用dma_map_single，自动设置cache flush，

CPU无法访问cache，这个是硬件自动完成的；但是CPU可以控制cache，使能cache为非法(破坏命中)，会自动与memory同步。

DMA Streaming Mapping

dma_addr_t dma_map_single(...);

void dma_unmap_single (...);

有的强大DMA，不需要连续内存做DMA操作，scater/getter可以用

int dma_map_sg(...);

void dma_unmap_sg (...);

参考《关于DMA和dma_alloc_coherent若干误解的澄清》

有一些强大硬件，DMA能感知cache网络互联；

这3套API，对任何硬件都成立；实现API后端，兼容不同硬件。

有些新的强大硬件，有支持IOMMU/smmu

![](media/33f4cf825d818ceeb55f2bed46ce02fe.png)

Dma可以从不连续内存，CMA申请内存

dma_alloc_coherent不需要从CMA申请内存，支持MMU，可以用物理不连续的内存来实现DMA操作；

![](media/2b61657481e8532ccffea663698f99f0.jpg)

## 2. 内存的cgroup

./swapoff –a

echo 1 \> /proc/sys/vm/overcommit_memory //

/sys/fs/cgroup/memory

mkdir A

cd A

sudo echo \$(100\*1024\*1024) \> memory.limit_in_bytes //限制最大内存100M

//a.out放到A cgroup执行；

sudo cgexec –g memory:A ./a.out

## 3. 性能方面的调优：page in/out, swapin/out

## 4. Dirty ratio的一些设置

脏页写回

时间维度：时间到，脏页写回；dirty_expire_centisecs

空间维度：

Dirty_ratio

Dirty_background_ratio:

cd /proc/sys/vm

![](media/1703771327809b1776192a4d948d96c9.png)

假如某个进程在不停写数据，当写入大小触发dirty_background_ratio_10%时，脏页开始写入磁盘，写入数据大小触发dirty_ratio_20%时(磁盘IO速度远比写内存慢)，如果继续写，会被内核阻塞，等脏页部分被写入磁盘，释放pagecached后，进程才能继续写入内存；

![](media/d9fa8494584d1c5e6e286e6bd120a90d.jpg)

所有的脏页flush，都是后台自动完成，当写入速度不足以触发dirty_ratio时，进程感知不到磁盘的存在。

## 5. swappiness

内存回收reclaim，涉及三个水位，

![](media/ebd0ac95b88659438744f1a21af8efa1.png)

Iph8.jpg

![](media/eff843ed97e04c51ba9c21f2f22cc164.jpg)

回收哪里的内存,back_groudpages, swap，由swappiness决定

![](media/8c7d0fb3d337d001d1f4739d49adf20d.png)

Swappiness越大，越倾向于回收匿名页；

即使Swappiness设置为0，先回收背景页，以使达到最低水位；假使达不到，还是会回收匿名页；

**有个例外**，对于cgroup而言，设置swappiness=0，该group的swap回收被关闭；但是全局的swap还是可以回收；

为什么设定最低水位，在Linux有一些紧急申请，PF_MEMALLOC，可以突破min水位，(回收内存的过程，也需要申请内存，类似征粮队自身需要吃粮食)。

最低水位，可以通过lewmem_reserve_ratio修改；

![](media/342b3dba400ad3fc2112fa8fc9097fcd.png)

关于最低水位的计算方法：

![](media/861cb980fbf3200617f4e1a8c7e58dd1.png)

回收inode/dentry的slab

![](media/2f53631c6abc21d2a5d4209990bc911a.png)

设置drop_caches

![](media/a1907b75740ab025c86b61d706e02627.png)

![](media/6a05756892b0222976274d5f73ea0a8e.png)

getdelays工具

getdelays 测量调度、I/O、SWAP、Reclaim的延迟，需要内核打开相应选项

CONFIG_TASK_DELAY_ACCT=y

CONFIG_TASKSTATS=y

Documentation/accounting/delay-accounting.txt

![C:\\Users\\tvt\\AppData\\Local\\Temp\\企业微信截图_15985180591094.png](media/57600f157380e0dde480e5c2cbc31d8c.png)

vmstat

用法,man vmstate

模糊查找

apropos timer

(base) leon\@pc:\~/work/myHub/linux/kernel/linux/Documentation/accounting\$
apropos vmsta

vmstat (8)      - Report virtual memory statistics

(base) leon\@pc:\~/work/myHub/linux/kernel/linux/Documentation/accounting\$

![](media/a5f2131bcf953ad13c8bdef51c6af778.png)

课后阅读

https://github.com/21cnbao/memory-coureses

<https://access.redhat.com/documentation/enus/red_hat_enterprise_linux/6/html/performance_tuning_guide/s-memory-tunables>
