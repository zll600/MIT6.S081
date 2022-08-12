# 这次实验的指导书在 [这里](https://pdos.csail.mit.edu/6.828/2020/labs/lazy.html)

## Eliminate allocation from sbrk()

这个任务比较简单，就是在 `sbrk()` 中，删除分配内存的那一部分代码，只改变 `myproc()->sz`，而不是真的去 alloc memory，就按照提示照做就可以了，问题不大，

这里主要是因为 growproc() 会立即分配 physical memory，并立即建立映射，所以需要取消这里

## lazy allocation

这个人物跟着提示做，还是能做出来的
- 如果 r_scause() 的返回值是 13 或者是 15，那么就可以判定是否发生了 page fault
- 根据 r_stval() 的返回值，可以确定是访问到那部分地址的时候，发生了 page fault
- 使用 PGROUNDDOWN() 来处理 va 值，如果你想多学一点知识，可以试着省略这一步，然后你就会遇到问题了，这步并不是可有可无
- 执行完上面的部分后，缺页中断可以正常恢复了，但是当 release 的时候，可能还是会出现问题，所以需要在 uvmunmap 中，当 page 没有建立相应的 map 时，不需要 panic，直接 continue 就可以了

## lazytests and usertests

1. 处理当 n < 0 时，sbrk() 可以正常处理，这里可以参考原本 growproc() 的逻辑，直接调用 uvmdealloc 释放内存就可以了
2. 在发生 page fault 时，不能为超过当前地址空间的 va 建立映射，这里需要在 kernel/trap.c 中修改
3. 在 fork 的时候，需要将 parent 的地址空间拷贝给 children，这时对于 parent 中，那些 lazy allocation 的 page ，直接跳过就可以了
4. 如果内存不足，kalloc() fails，就直接kill，然后退出
5. 如果访问到低于 user stack 的地址，就直接 kill，然后退出，这里需要取的 user mode 下的 user stack 的地址，在处理中断的时候，sp 因该是暂存在 trapframe 中了，因此可以通过 p->trapframe->sp 来获取 
6. 当发生 read/write 这类系统调用时，此时已经发生系统调用，已经切换到 kernel page table 了，所以如果穿进来的虚拟地址没有对应的物理地址的话，在 walk 中就会 panic（回顾之前，kernel 中需要将来子 user space 中的 va 转化为 pa，并建立相关映射才可以继续使用）

这里的调用链路大致是 write --> sys_write --> writei --> either_copyin --> copyin --> walkaddr ，需要对 walkaddr 做出一定的修改才可以

## reference
- https://blog.csdn.net/rocketeerLi/article/details/121738127
- https://blog.miigon.net/posts/s081-lab5-lazy-page-allocation/
- http://www.manongjc.com/detail/25-eibkqdatacnermn.html
