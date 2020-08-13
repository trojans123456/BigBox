#### HASH函数

hash操作目的就是将一串数据通过hash来生成固定的值。
		查找速度O(1)

##### hash函数分类

###### 1.加法Hash

​    所谓的加法Hash就是把输入元素一个一个的加起来构成最后的结果

```
1  static int additiveHash(String key, int prime)
2  {
3   int hash, i;
4   for (hash = key.length(), i = 0; i < key.length(); i++)
5    hash += key.charAt(i);
6   return (hash % prime);
7  }
```

###### 2.位运算Hash

​    通过利用各种位运算（常见的是移位和异或）来充分的混合输入元素

```
1  static int rotatingHash(String key, int prime)
2  {
3    int hash, i;
4    for (hash=key.length(), i=0; i<key.length(); ++i)
5      hash = (hash<<4)^(hash>>28)^key.charAt(i);
6    return (hash % prime);
7  }
```

 变形

1. ```
   1. hash = (hash<<5)^(hash>>27)^key.charAt(i);
   2. hash += key.charAt(i);
       hash += (hash << 10);
       hash ^= (hash >> 6);
   3. if((i&1) == 0)
       {
        hash ^= (hash<<7) ^ key.charAt(i) ^ (hash>>3);
       }
       else
       {
        hash ^= ~((hash<<11) ^ key.charAt(i) ^ (hash >>5));
       }
   4. hash += (hash<<5) + key.charAt(i);
   5. hash = key.charAt(i) + (hash<<6) + (hash>>16) – hash;
   6. hash ^= ((hash<<5) + key.charAt(i) + (hash>>2));
   ```

   

###### 3.乘法Hash

​    利用了乘法的不相关性（乘法的这种性质，最有名的莫过于平方取头尾的随机数生成算法，虽然这种算法效果并不好）
​    字符串特别适应

```
1  static int bernstein(String key)
2  {
3    int hash = 0;
4    int i;
5    for (i=0; i<key.length(); ++i) hash = 33*hash + key.charAt(i);
6    return hash;
7  }
```

###### 4.数组Hash

```
1 static inline int hashcode(const int *v)
2 {
3  int s = 0;
4  for(int i=0; i<k; i++)
5     s=((s<<2)+(v[i]>>4))^(v[i]<<10);
6  s = s % M;
7  s = s < 0 ? s + M : s;
8  return s;
9 }
```

哈希冲突的解决
选用哈希函数计算哈希值时，可能不同的 key 会得到相同的结果，一个地址怎么存放多个数据呢？这就是冲突

1.链接法(拉链法)

![1597290885214](C:\Users\didi\AppData\Roaming\Typora\typora-user-images\1597290885214.png)

2.开放定址法

​	a线性探测

​    hi = (h(key) + i) %m,