# lab 1 

### sleep(easy)

这个比较简单，注意一下对命令行参数的处理，然后直接调用系统调用即可

通过这个例子可以学到的有：
- 命令行输入的参数都是字符串即： char* 类型，而在调用相关函数的时候，经常需要使用 `atoi` 等类型转化函数
- user/user.h 中的 sleep 是在 user/sys.S 中实现的，然后跳转到 kernel/sysproc.c 中真正执行，可以借此理解从用户态切换到内核态的过程

### pingpong(easy)

这个例子中在理解 fork 产生的父子进程以及 pipe 的使用

这里可以参考 [xv6book](https://pdos.csail.mit.edu/6.828/2020/xv6/book-riscv-rev1.pdf) 的第一章的部分

### find(moderate)

这个例子中就暗号 user/ls.c 来写就差不多，理解一下文件系统，这里的话，其实的不同在于，在查找的过程中，一旦找到就直接返回就可以了

### xargs(moderate)

这里利用一个字符串数组来存储 exec 需要的参数，如果 xargs 后有额外的参数，这里需要提前写入，并且不能动
