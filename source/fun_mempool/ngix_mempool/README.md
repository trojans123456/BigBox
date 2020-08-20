
Examples
========

    1 #include "ncx_slab.h" 
    2 
    3 int main(int argc, char **argv)
    4 {
    5     char *p;                                                                                                               
    6     size_t  pool_size = 4096000;  //4M                                                                                     
    7     ncx_slab_stat_t stat;                                                                                                  
    8     u_char  *space;                                                                                                        
    9     space = (u_char *)malloc(pool_size);                                                                                   
    10                                                                                                                            
    11     ncx_slab_pool_t *sp;                                                                                                   
    12     sp = (ncx_slab_pool_t*) space;                                                                                         
    13                                                                                                                            
    14     sp->addr = space;                                                                                                      
    15     sp->min_shift = 3;                                                                                                     
    16     sp->end = space + pool_size;                                                                                           
    17                                                                                                                            
    18     /*                                                                                                                     
    19      * init                                                                                                                
    20      */                                                                                                                    
    21     ncx_slab_init(sp);                                                                                                     
    22                                                                                                                            
    23     /*                                                                                                                     
    24      * alloc                                                                                                               
    25      */                                                                                                                    
    26     p = ncx_slab_alloc(sp, 128);                                                                                           
    27                                                                                                                            
    28     /*                                                                                                                     
    29      *  memory usage api. 
    30      */                                                                                                                    
    31     ncx_slab_stat(sp, &stat);                                                                                              
    32                                                                                                                            
    33     /*                                                                                                                     
    34      * free                                                                                                                
    35      */                                                                                                                    
    36     ncx_slab_free(sp, p);                                                                                                  
    37                                                                                                                            
    38     free(space);                                                                                                           
    39                                                                                                                            
    40     return 0;
    41} 
   
API
===
**ncx_slab_init(ncx_slab_pool_t *pool)** <br/>
**Description**: 初始化内存池结构；

**ncx_slab_alloc(ncx_slab_pool_t *pool, size_t size)**<br/>
**Description**: 内存分配

**ncx_slab_free(ncx_slab_pool_t *pool, void *p)** <br/>
**Description**: 释放内存

**ncx_slab_stat(ncx_slab_pool_t *pool, ncx_slab_stat_t *stat)**<br/>
**Description**: 查看内存池使用情况

Customization
=============
正如example所示，内存池内存是由应用层先分配，ncx_mempool是在给定的内存基础上进行分配和回收管理。 <br/>
所以内存池本身不关心所管理的内存是私有内存 or 共享内存; 

ncx_lock.h 是锁接口；根据实际需要重定义: <br/>
1.多线程共享内存池，可参考pthread_spin_lock <br/>
2.多进程共享内存池，可参考nginx的ngx_shmtx.c实现spin lock <br/>
3.单进程单线程使用内存池，无锁编程..

ncx_log.h 是日志接口，根据实际需要重定义.

ncx_slab.c.orz 是 ncx_slab.c的详细注释，方便理解.
