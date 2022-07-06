实验手册在 [这里](https://pdos.csail.mit.edu/6.828/2020/labs/pgtbl.html)

xv6 中的实现是，每个用户态进程都有一个 pagetable，但是内核态进程公用一个 pagetable，这两者从虚拟地址到物理地址的映射规则是不一样的，

- 用户态，进行地址空间从 0 开始
- 内核态，内核代码和数据从 0x80000000 开始，低于这个地址的是主板上的其他设备，

### Print a page table

这个人物就是打印第一个启动的进程 virtual address 到 physical address 的映射

- 可以使用 kernel/riscv.h 中的宏
- 可以参考 freewalk 中的实现

### A kernel page table per process

这个任务是因为所有的内核态进程的页表都是一样的，这里为了使得用户态进程切换到内核态时，可以使得每个内核进程都会有一个内核页表

- 在 kerner/proc.h 中修改 struct proc，增加 `kernel_pgtbl` field
- 根据 kernel/proc.c 中 `allocproc` 中生成一个 user mode 的 `pagetable` 的过程，因为内核页表的映射规则应该按照之前的全局内核页表的方式来做，所以在在 `kernel/vm.c` 中增加 `ukmcreate` 函数，里面的内容根据 `kinit` 函数来写，根据 `kvmmap` 来编写 `ukvmmap` 函数
- 根据 `kernel/proc.c` 中 procinit 中关于初始化 kernel stack 的代码，在 allocproc 中编写 初始化 `p->kernel_pgtbl` 的 kernel stack 的代码，之前所有的内核进程共享一个全局的内核页表，所以所有的内核进程的内核栈的映射使用同一张页表，这里看之查看 [xv-book](https://pdos.csail.mit.edu/6.828/2020/xv6/book-riscv-rev1.pdf)
- 在 scheduler() 函数中 参考 kvminithart 添加 设置 内核态进程 satp 寄存器的代码，在切换内核态进程的 satp 寄存器页表，
- 在 freeproc 中释放 内核页表，这里注意只需要释放内核页表，而不用释放物理页，首先释放内核页表的内核栈，不过需要首先将内核栈的虚拟地址转化为物理地址才可以，这里利用 kvmpa 重新编写一转化的函数，然后得到内核栈对应的物理地址，之后通过 kfree 释放，接着根据 freewalk 函数，释放内核页表，但是不可以释放内核使用的内存页，只是删除页表中的映射关系而已
- 这里因为需要还需要对 `virtio_disk.c` 中使用 kvmpa 处进行修改，改为使用 ukvmpa 来代替

### Simplify copyin/copyinstr

这个任务主要做的就是将用户态 page table 中的映射添加到 kernel page table 中，使得 copyint 可以直接解引用用户态的指针

- 增加 u2kvmcopy 函数，将 user page table 中的映射添加到 kernel page table 中，具体的做法就是 利用 user page table 找到对应的 physical address，然后在 kernel page table 中增加新的 映射，可以参考 uvmcopy 函数，做适当修改就可以了
- 接下里利用这些工具函数，修改fork, exe, sbrk 和 userinit 中操作 user page table 中的地方，在操作 user page table 的同时，同时操作 kernel page table, 这里还需要增加一个工具函数 kvmdealloc 用来处理 kernel memory page 的缩容
- 修改 copyin 和 copyinstr 的实现
