#ifndef __GLOBAL_H
#define __GLOBAL_H

/**
*有些类不允许复制对象。这些
类包含私有的复制构造函数和赋值
操作符禁用复制(编译器给出错误消息)

class xxx
{
    public:

    G_DISABLE_COPY(xxx)
};
*/
#define G_DISABLE_COPY(Class) \
    private: \
    Class(const Class &); \
    Class &operator=(const Class &);


#define G_BEGIN_NAMESPACE(name) \
    namespace name {

#define G_END_NAMESPACE(name) \
    }

#define G_C_BEGIN_HEADER \
    extern "C" {

#define G_C_END_HEADER \
    }

#endif
