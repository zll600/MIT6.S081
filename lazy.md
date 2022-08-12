# 这次实验的指导书在 [这里](https://pdos.csail.mit.edu/6.828/2020/labs/lazy.html)

## Eliminate allocation from sbrk()

这个任务比较简单，就是在 `sbrk()` 中，删除分配内存的那一部分代码，只改变 `myproc()->sz`，而不是真的去 alloc memory，就按照提示照做就可以了，问题不大，

## lazy allocation

这个人物跟着提示做，还是能做出来的
- 如果 r_scause() 的返回值是 13 或者是 15，那么就可以判定是否发生了 page fault
- 根据 r_stval() 的返回值，可以确定是访问到那部分地址的时候，发生了 page fault
- 使用 PGROUNDDOWN() 来处理 va 值，如果你想多学一点知识，可以试着省略这一步，然后你就会遇到问题了，这步并不是可有可无
- 执行完上面的部分后，缺页中断可以正常恢复了，但是当 release 的时候，可能还是会出现问题，所以需要在 uvmunmap 中，当 page 没有建立相应的 map 时，不需要 panic，直接 continue 就可以了

## reference
- https://blog.csdn.net/rocketeerLi/article/details/121738127
- https://blog.miigon.net/posts/s081-lab5-lazy-page-allocation/
