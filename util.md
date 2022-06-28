# lab 1 

### sleep(easy)

这个比较简单，注意一下对命令行参数的处理，然后直接调用系统调用即可

通过这个例子可以学到的有：
- 命令行输入的参数都是字符串即： char* 类型，而在调用相关函数的时候，经常需要使用 `atoi` 等类型转化函数
- user/user.h 中的 sleep 是在 user/sys.S 中实现的，然后跳转到 kernel/sysproc.c 中真正执行，可以借此理解从用户态切换到内核态的过程

### pingpong(easy)

这个例子中在理解 fork 产生的父子进程以及 pipe 的使用

### find(moderate)

这个例子中就暗号 user/ls.c 来写就差不多，理解一下文件系统

