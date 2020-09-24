#ifndef __WIN32__
#include <sys/stat.h>
#include <unistd.h>
#endif
#include "dir.h"

using namespace ns_core;

dir::dir(const string &path = ""):cur_path(path)
{
    if(cur_path.empty())
        cur_path = ".";
}

dir::~dir()
{

}

bool dir::mkdir(const string &dirName) const
{
    string path = cur_path + "/" + dirName;
    return ::mkdir(path.c_str(),S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == 0;
}

bool dir::mkdir(const string &path)
{
    return ::mkdir(path.c_str(),S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == 0;
}

bool dir::rmdir(const string &dirName) const
{
    string path = cur_path + "/" + dirName;
    return ::rmdir(path.c_str()) == 0;
}

bool dir::rmdir(const string &path)
{
    return ::rmdir(path.c_str()) == 0;
}

bool dir::exists(string &path) const
{
    return ::access(path.c_str(),F_OK) == 0;
}

bool dir::remove(const string &fileName)
{
    return ::remove(fileName.c_str()) == 0;
}
