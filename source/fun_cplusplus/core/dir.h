#ifndef __DIR_H
#define __DIR_H

#include <string>

using std::string;

namespace ns_core
{

class dir
{
public:
    dir(const string &path = "");
    ~dir();

    bool mkdir(const string &dirName) const;
    static bool mkdir(const string &path);

    bool rmdir(const string &dirName) const;
    static bool rmdir(const string &path);

    static bool exists(string &path) const;

    bool remove(const string &fileName);
private:
    string cur_path;
};

}

#endif
