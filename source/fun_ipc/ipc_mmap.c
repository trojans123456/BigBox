#include <sys.mman.h>

/**
void *mmap(void*start,size_t length,int prot,int flags,int fd,off_t o
ffset); //mmap 函数将一个文件或者其它对象映射进内存。 第一个参数为映射区的开始
地址， 设置为 0 表示由系统决定映射区的起始地址， 第二个参数为映射的长度， 第三个参
数为期望的内存保护标志， 第四个参数是指定映射对象的类型， 第五个参数为文件描述符
（指明要映射的文件） ， 第六个参数是被映射对象内容的起点。 成功返回被映射区的指针，
失败返回 MAP_FAILED[其值为(void *)-1]。

int munmap(void* start,size_t length); //munmap 函数用来取消参数 start 所
指的映射内存起始地址， 参数 length 则是欲取消的内存大小。如果解除映射成功则返回 0，
否则返回－1， 错误原因存于 errno 中错误代码 EINVAL。

 int msync(void *addr,size_t len,int flags); //msync 函数实现磁盘文件内容
和共享内存取内容一致， 即同步。 第一个参数为文件映射到进程空间的地址， 第二个参数
为映射空间的大小， 第三个参数为刷新的参数设置。

*/
