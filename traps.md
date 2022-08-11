先放一下这次的 [实验指导书](https://pdos.csail.mit.edu/6.828/2020/labs/traps.html)

这里概括一下我自己对于整个中断过程的理解

这里先说明一点，计算机启动的时候首先是在 machine mode 下，在这里初始化一些硬件配置，将操作系统加载到内存中，然后切换到 supervisor mode 将计算机的操作权交给了操作系统，之后操作系统做完必要的工作后才切换到 user mode，将 uservec 的地址放入 stvec 中

后面就是书中所说的部分，当在 user mode 下发生 trap 时（比如： system call），先通过 call 切换到 supervisor mode，这是 cpu 会跳转到 stvec 中的位置，开始执行这里的代码，也就是先进入 userstvec（即 trampoline.S处），将所有的寄存器保存到tramframe 中，然后是 usertrap，接着执行指定的系统调用，返回的时候先调用 usertrapret 中，然后跳转到 userret 中，最后切换回 user mode，

这里需要注意 ecall 只做什么

- ecall 只是从 user mode 切换到 super mode
- 保存当前寄存器的值，然后将 pc 值设置为 kernel 中的值

ecall 没有切换 page table 和 kernel stack


## RISC-V assembly

任务一的我自己的想法也写在 answers-traps.txt 中了，仔细看一看 lecture 就问题不大了

## Backtrace

在理解这里的内容的时候，一定要考虑到 栈是向低地址方向增长的，看讲义的时候也需要记住这一点

编译器来每个栈帧中放入一个栈帧指针，该指针保存着 caller 函数的栈帧指针的地址，你的 backtrace 应该使用栈帧指针回溯整个调用栈，并打印出每个栈的调用 返回地址

1. 在 kernel/defs.h 中添加 void backtrace(void) 的函数声明，使得 `sys_sleep` 中可以调用 backtrace
2. 在 kernel/riscv.h 中添加这段代码用于获取当前函数的栈指针（这里用到了 gcc 的内联汇编的形式），gcc 编译器将执行函数的栈指针存放在 s0 寄存器中，利用内联汇编读出 s0 的值，称为 fp

````c
static inline uint64
r_fp()
{
  uint64 x;
  asm volatile("mv %0, s0" : "=r" (x) );
  return x;
}
````
3. 根据 [lecture-note](https://pdos.csail.mit.edu/6.828/2020/lec/l-riscv-slides.pdf)，函数的返回值是固定放在 fp-8 的位置，caller 函数的栈帧指针是放在 fp-16 中，
4. xv6 在内核中 PAGE-aligned 的地址处，为每一个函数分配一个内存页，你可以利用 PGROUNDUP() 和 PGROUNDDOWN() 来确定栈的上界和下界

这里注意题目要求

## Alarm

这个人物就是在 xv6 中新增加一个 feature，在进程使用 CPU 时间的时候定期提醒它，也就是用一种较为原始的形式实现用户级中断/故障处理程序，

1. 修改 Makefile，增加编译 alarmtest.c
2. 在 user/user.h 中增加新的函数声明
````c
int sigalarm(int ticks, void (*handler)());
int sigreturn();
````

3. 修改 user/usys.pl，kernel/syscall.h 和 kernel/syscall.c 确保 alarmtest 可以调用 sigalarm 和 sigreturn 函数，就按照之前添加系统调用号的流程来做
4. 先将 sigreturn 的返回值置零
5. sys_sigalarm 应该保存 internal 和 handler，并应该新 struct proc 中添加新的 field 来保存这些，并且在 allocproc(kernel/proc.c) 中进行初始化，同样在 freeproc 中置零
6. 在 usertrap 中修改对于时间中断的处理逻辑，在这里每次定时器到期后，重置定时器，同时要让 handler 函数得以执行，这里需要注意的是，handler 这个函数是在 user mode 下的函数，所以必须在使用 user page table 的前提下进行调用，而在 system call 执行结束后在 sret (usertrapret 中)，后使用 trapframe 中的值来恢复 pc 的值，所以在这里还需要将 trapframe 中 epc 的值修改为 handler 函数的地址，这样 handler 函数就可以在 user mode 下执行， 
7. test1 中有提示，如果无法正常通过，可能是因为 handler 执行结束后，没有回到 interrupt 发生的位置继续执行，或者调用 handler 时，使用了 interrupt 发生时保存的 registers，导致后面循环使用了错误的值
8. test2 中有提示是防止同时有多个 handler 处理函数在执行8. test2 中有提示是防止同时有多个 handler 处理函数在执行8. test2 中有提示是防止同时有多个 handler 处理函数在执行8. test2 中有提示是防止同时有多个 handler 处理函数在执行，这里先通过查看 test2 的代码，来尝试理解为什么，
这里是用过使用父子进程之间的关系，企图让 handler 函数多次执行，并在 slow_handler 中进行等待，制造 reentrant 的机会，这里的解决方法就是在处理时间中断的时候，如果当前 ticks 还没到期，就不能处理再次调用 handler 函数，而只有在 sigreturn 系统调用返回之后，才将定时器重置，确保不会发生重入的情况，
9. test1 和 test2 其实才体现出来 sigreturn 的作用

### reference
- https://blog.miigon.net/posts/s081-lab4-traps/#backtrace-moderate
- https://blog.csdn.net/rocketeerLi/article/details/121665215
