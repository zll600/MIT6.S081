这次的实验地址在 [这里](https://pdos.csail.mit.edu/6.828/2020/labs/cow.html)

这次实验只有一个任务就是完成 copy-on-write 这个机制

1. 首先这个实验和上一个 lazy-allocation 的实验是有一定相似的地方的，所以这里可以从里面借鉴一些思路，比如，如何判断 page fault，以及确定发生 page fault 的地址，

2. 对于每个 pte 现在必须再加上以为来判断这个页是否是 cow 页，这里可以利用 pte 的 RSW 来做，我这里使用的是第 8 位

> #define PTE_COW(1 << 8)

3. 对于每个cow 页，需要维护一个引用计数，只有当引用计数为 0 的时候，才会 free page，那么这里还涉及倒一个为题，那些页可能作为 cow 页呢，提示有说可以参考 kernel/riscv.h，里面定义了许多的 macro，可以发现 kernel 的地址空间就是 PHYSTOP - KERNBASE 这个范围，所以这里的 index 数组，长度就是 (PHYSTOP - KERNBASE) / PGSIZE 

> uint page_ref[(PHYSTOP - KERNBASE) / PGSIZE];

同时这里还需要添加一个用来确定给定 physical address 所在的数组 index

> #define COW_INDEX(pa) (((uint64)(pa) - KERNBASE) >> PGSHIFT)

这里同时需要注意，宏中的变量尽可能的用括号括起来了，避免当出现表达式出问题

4. 
