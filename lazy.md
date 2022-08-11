# 这次实验的指导书在 [这里](https://pdos.csail.mit.edu/6.828/2020/labs/lazy.html)

## Eliminate allocation from sbrk()

这个人物比较简单，就是在 `sbrk()` 中，删除分配内存的那一部分代码，只改变 `myproc()->sz`，而不是真的去 alloc memory，就按照提示照做就可以了，问题不大，

## reference

