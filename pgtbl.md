实验手册在 [这里](https://pdos.csail.mit.edu/6.828/2020/labs/pgtbl.html)

xv6 中的实现是，每个用户态进程都有一个 pagetable，但是内核态进程公用一个 pagetable，这两者从虚拟地址到物理地址的映射规则是不一样的，

- 用户态，进行地址空间从 0 开始
- 内核态，内核代码和数据从 0x80000000 开始，低于这个地址的是主板上的其他设备，

### Print a page table

这个人物就是打印第一个启动的进程 virtual address 到 physical address 的映射

- 可以使用 kernel/riscv.h 中的宏
- 可以参考 freewalk 中的实现，最后一层页表项中，r,w,x 中一定有以为是 1

### A kernel page table per process

这个任务是因为所有的内核态进程的页表都是一样的，而内核进程的页表中是没有这些映射的，所以kernel 无法直接使用 user process 的 virtual address，所以kernel 想要使用 user process 的 virtual address 的时候必须先将 virtual address 转化为 physical address。所以这里要做的修改就是修改 kernel 使得，使得每个进程在 kernel 中执行时都是用的是自己的 kernle page table 的副本，修改 scheduler 中的代码，在 switch process 的时候同时 switch kernel page table ：

- 在 kerner/proc.h 中修改 struct proc，增加 `kernel_pgtbl` field
- 根据 kernel/vm.c 中的 kvminit 函数编写实现一个函数生成一个新的 page table，而不是修改全局的 `kernel_pagetable` ，这个函数可能在 kernel/proc.c 中的 allocproc 中调用，具体就是增加一个 ukvmcreate 函数，和 ukvmmap 函数来实现在 kvminit 中初始化 `kernel_pagetable` 的逻辑
- 根据 `kernel/proc.c` 中 procinit 中关于初始化 kernel stack 的代码，在 allocproc 中编写 初始化 `p->kernel_pgtbl` 的 kernel stack 的代码， 必须是哟哦个自己的映射函数来将 virtual address 映射到 physical address 中
- 在 scheduler() 函数中 参考 kvminithart 设置 kernel page 的地址到 satp 寄存器的代码，在调用 `w_stap` 后还需要调用 `sfence_vma` 来清楚页表缓存
- 在 freeproc 中释放一个进程的 kernel pagetable，这里注意只需要释放内核页表，而不用释放物理页，这部分可以参考 释放用户进程的 `proc_freepagetable` 函数的逻辑来写，但是一定不能释放 kernel pagetable 的 物理页这
    - 首先释放内核页表的内核栈，这里需要使用 kfree 这个函数，但是这个函数需要的是 physical address，需要首先将 kstack 的 virtual address 转化 physical address 为才可以，这里利用 kvmpa 重新编写一转化的函数，然后得到内核栈对应的物理地址，之后通过 kfree 释放，接着根据 freewalk 函数，释放内核页表，但是不可以释放内核使用的物理页，进程的 kernel page table 只是一层重复信息（这里可以参考 kernel 的 virtual address 到 phical aaddress 的映射方式），不应该释放真实的物理页只是删除页表中的映射关系而已，也就是不能释放最后一层
- 这里因为需要还需要对 `virtio_disk.c` 中使用 kvmpa 处进行修改，改为使用 ukvmpa 来代替

### Simplify copyin/copyinstr

copyin 这个函数之前读取 user space 下的地址时，必须先将这个地址转化为 physical address 才可以，这个任务主要做的就是将用户态 page table 中的映射添加到 kernel page table 中，使得 copyint 可以直接解引用用户态的指针，这个人物依赖于 user space 的 page table 不会和 kernel page table 发生 overlap，必须避免 user space 的地址增长超过 PLIC 即 0X0C000000 ，这节有依赖上一节做的 kernel page table

- 在每一处需要修改 user page table mapping 的地方修改 kernel page talbe 的 mapping
- 增加 u2kvmcopy 函数，将 user page table 中的映射添加到 kernel page table 中，具体的做法就是 利用 user page table 找到对应的 physical address，然后在 kernel page table 中增加新的 映射，可以参考 uvmcopy 函数，做适当修改就可以了，大致过程就是利用 user page table 获得 physical address 然后利用 mappages 建立 virtual address 和 physicall address 的 mapping 
- 接下里利用这些工具函数，修改fork, exe, sbrk 和 userinit 中操作 user page table 中的地方，在操作 user page table 的同时，同时操作 kernel page table, 这里还需要增加一个工具函数 kvmdealloc 用来处理 kernel memory page 的缩容
- 修改 copyin 和 copyinstr 的实现

学到的知识:

进程可以同时拥有 user page table 和 kernel page table 的，当 user mode 下的 process 发起 system call 时，process 切换到 kernel mode 中执行，使用 kernel page table，可以减少上下文切换的 开销
