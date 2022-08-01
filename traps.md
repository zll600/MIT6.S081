先放一下这次的 [实验指导书](https://pdos.csail.mit.edu/6.828/2020/labs/traps.html)

## RISC-V assembly

任务一的我自己的想法也写在 answers-traps.txt 中了，仔细看一看 lecture 就问题不大了

## Backtrace

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
3. 根据 [lecture-note](https://pdos.csail.mit.edu/6.828/2020/lec/l-riscv-slides.pdf)，函数的返回值是固定放在 fp-8 de 位置，caller 函数的栈帧指针是放在 fp-16 中，
4. xv6 在内核中 PAGE-aligned 的地址处，为每一个函数分配一个内存页，你可以利用 PGROUNDUP() 和 PGROUNDDOWN() 来确定栈的上界和下界

这里注意题目要求


