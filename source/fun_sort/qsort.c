#include <stdio.h>
#include "qsort.h"

/**
 * @brief 数据交换
 * @param base
 * @param size
 * @param a
 * @param b
 */
static void exch(char* base,int size,int a,int b) {
  char* x=base+a*size;
  char* y=base+b*size;
  while (size) {
    char z=*x;
    *x=*y;
    *y=z;
    --size; ++x; ++y;
  }
}

/** 快排 */
static void quicksort(char* base,int size,int l,int r,
              int (*compare)(const void*,const void*)) {
  int i=l-1, j=r, p=l-1, q=r, k;
  char* v=base+r*size;
  if (r<=l) return;
  for (;;) {
    while (++i != r && compare(base+i*size,v)<0) ;
    while (compare(v,base+(--j)*size)<0) if (j == l) break;
    if (i >= j) break;
    xj_exch(base,size,i,j);
    if (compare(base+i*size,v)==0) xj_exch(base,size,++p,i);
    if (compare(v,base+j*size)==0) xj_exch(base,size,j,--q);
  }
  xj_exch(base,size,i,r); j = i-1; ++i;
  for (k=l; k<p; k++, j--) xj_exch(base,size,k,j);
  for (k=r-1; k>q; k--, i++) xj_exch(base,size,i,k);
  quicksort(base,size,l,j,compare);
  quicksort(base,size,i,r,compare);
}

/**
 * @brief qsort
 * @param base
 * @param nmemb  数组个数
 * @param size   数组内容大小
 * @param compare 如何比较
 */
void qsort(void* base,int nmemb,int size,int (*compare)(const void*,const void*)) {
  /* 检测是否超过 int大小*/
  if (nmemb >= (((unsigned int)-1)>>1) || size >= (((unsigned int)-1)>>1))
  {
      printf("------ %d\n",(((unsigned int)-1) >> 1));
      return;
  }
  /*快排*/
  if (nmemb>1)
    quicksort(base,size,0,nmemb-1,compare);
}
