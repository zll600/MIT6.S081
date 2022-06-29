### System call tracing

1. user/trace.c 已经存在在，先在 Makefile 中添加对应的条目
2. 在 user/user.h 中添加 prototype
3. 在 user/usys.pl 中添加对应的 entry，生成 user/usys.S 生成对应的 ecall 指令，从 user mod 跳转到 supervisor mode
4. 在 kernel/syscall.h 中添加对应的系统中断号，并通过中断号索引对应的中断处理函数
5. 在 kernel/sysproc.c 中添加具体实现 `sys_trace()` 函数，在 kernel/syscall.c 中添加对应的中断处理函数
5. 修改 kernel/proc.c 中的 fork 函数，时的子进程可以继承父进程的 `trace_mask`
6. 修改 kernel/syscall.c 中的 syscall 函数，并新增一个 `sysall_names` 数组来通过中断号索引系统中断的名称，这里利用寄存器获取中断号，并将处理结果保存到寄存器中

这里需要梳理一下系统调用的整个执行流程
