### System call tracing

1. user/trace.c 已经存在在，先在 Makefile 中添加对应的条目
2. 在 user/user.h 中添加 prototype
3. 在 user/usys.pl 中添加对应的 entry，生成 user/usys.S 生成对应的 ecall 指令，从 user mod 跳转到 supervisor mode
4. 在 kernel/syscall.h 中添加对应的系统中断号，并通过中断号索引对应的中断处理函数
5. 在 kernel/sysproc.c 中添加具体实现 `sys_trace()` 函数，在 kernel/syscall.c 中添加对应的中断处理函数
5. 修改 kernel/proc.c 中的 fork 函数，时的子进程可以继承父进程的 `trace_mask`
6. 修改 kernel/syscall.c 中的 syscall 函数，并新增一个 `sysall_names` 数组来通过中断号索引系统中断的名称，这里利用寄存器获取中断号，并将处理结果保存到寄存器中

实际上这里还有两个文件需要注意 kernel/trampoline.S 和 kernel/trap.c 这两个文件中的函数，会对从 user space 中的 argument 做验证，kernel 的返回值同样


### System info

1. user/user.h 中增加 `struct sysinfo` 和 `int sysinfo(struct sysinfo*)` 的声明
2. 在 user/usys.pl 中添加爱对应的 entry
3. 在 kernel/syscall.h 中增加对应的系统中断号，并在 kernel/syscall.c 中增加对应中断处理函数的声明
4. 在 kernel/kalloc.c 中增加 `uint64 free_mem(void)` 函数，用于计算空闲内存页的大小，这里注意 race condition 的处理，还需要在 kernel/defs.h 中增加函数声明
5. 在 kernel/proc.c 中增加 `uint64 proc_num(void)` 函数正在使用的进程的数目，
6. 在 kernel/sysproc.c 中实现 `sys_sysinfo` 中断处理函数，这里的难点在于需要将 一个 sysinfo  struct 返回到 user space，这里可以参考 kernel/file.c 中的 filestat 函数中的使用，返回的地址是通过 user space 中传递过来的，需要通过 argaddr 函数获取

查看内核的内分配部分后，可以发现，xv6 将空闲页表作为一个空闲链表存储起来，每次获取或者释放页面都只是操作这个链表而已

### reference

- https://blog.miigon.net/posts/s081-lab2-system-calls/#lab-2-system-calls
- https://blog.csdn.net/rocketeerLi/article/details/121263520
