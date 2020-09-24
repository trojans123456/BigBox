#ifndef __STRING_LIST_H
#define __STRING_LIST_H


#include <list>
#include <string>

using std::list;
using std::string;

namespace ns_core
{

class stringlist : public list<string>
{
public:
    stringlist() {}
    stringlist(const string &i) {append(i);}

    void sort();
    string join(const string &sep) const;

    stringlist operator+(const stringlist &other) const
    {
        stringlist n = *this;n += other;return n;
    }



    stringlist operator<<(const string &str)
    {
        append(str);return *this;
    }
    stringlist operator<<(const stringlist &l)
    {
        *this += l;return *this;
    }

    int indexOf(const string &str,int from = 0) const;
    int lastIndexOf(const string &str,int from = -1) const;
}


#endif
