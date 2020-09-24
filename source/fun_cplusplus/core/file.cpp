#include <unistd.h>
#include <stdio.h>
#ifndef __WIN32__
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <sys/mman.h>
#endif
#include "file.h"

using namespace ns_core;

OpenMode File::getOpenMode() const
{
    return mOpenMode;
}

void File::setOpenMode(OpenMode openMode)
{
    if(!isOpen())
    {
        mOpenMode = openMode;
    }
}

bool File::isOpen() const
{
    return mOpenMode != NotOpen;
}

bool File::isReadable() const
{
    return (getOpenMode() & ReadOnly) != 0;
}

bool File::isWriteable() const
{
    return (getOpenMode() & WriteOnly) != 0;
}

File::File():mFileName(""),mOpenMode(NotOpen),fd(-1)
{

}

File::File(const string &name):mFileName(name),mOpenMode(NotOpen),fd(-1)
{

}

File::~File()
{
    close();
}

string File::fileName() const
{
    return mFileName;
}
void File::setFileName(const string &name)
{
    if(!isOpen())
    {
        mFileName = name;
    }
}

bool File::exists() const
{
    return (access(mFileName.c_str(),F_OK) == 0);
}
bool File::exists(const string &filename)
{
    return (access(filename.c_str(),F_OK) == 0);
}

string File::readLink() const
{
    char buf[1024] = "";

    int64_t size = readlink(mFileName.c_str(),buf,sizeof(buf));
    if(size < 0)
    {
        return "";
    }
    return string(buf,size);
}

string File::readLink(const string &filename)
{
    char buf[1024] = "";

    int64_t size = readlink(filename.c_str(),buf,sizeof(buf));
    if(size < 0)
    {
        return "";
    }
    return string(buf,size);
}

bool File::remove()
{
    if(isOpen())
        close();
    return (remove(mFileName.c_str()) == 0);
}

bool File::remove(const string &filename)
{
    return (remove(filename.c_str()) == 0);
}

bool File::rename(const string &newName)
{
    if(newName.empty())
        return false;
    if(isOpen())
        close();

    return (rename(mFileName.c_str(),newName.c_str()) == 0);
}

bool File::rename(const string &oldName,const string &newName)
{
    if(oldName.empty() || newName.empty())
        return false;
    return (rename(oldName.c_str(),newName.c_str()) == 0);
}

bool File::link(const string &newName)
{
    if(newName.empty())
        return false;

    return (link(mFileName.c_str(),newName.c_str()) == 0);
}

bool File::link(const string &oldName,const string &newName)
{
    if(oldName.empty() || newName.empty())
        return false;

    return (link(oldName.c_str(),newName.c_str()) == 0);
}


bool File::open(OpenMode flags)
{
    int mode;
    switch(flags)
    {
    case ReadOnly:
        mode = O_RDONLY;
        break;
    case WriteOnly:
        mode = O_WRONLY;
        break;
    case ReadWrite:
        mode = O_RDWR;
        break;
    }

    fd = ::open(mFileName.c_str(),mode);
    if(fd < 0)
        return false;

    if(flags & Append)
    {
        seek(size());
    }
    setOpenMode(flags);

    return true;
}

bool File::open(FILE *f, OpenMode flags)
{
    return true;
}

bool File::open(int fd,OpenMode flags)
{
    return true;
}

void File::close()
{
    if(isOpen())
        ::close(fd);
}

uint64_t File::read(char *data,uint64_t maxlen)
{
    return ::read(fd,data,maxlen);
}

uint64_t File::write(char *data,uint64_t maxlen)
{
    return ::write(fd,data,maxlen);
}

uint64_t File::readLine(char *data,uint64_t maxlen)
{
#define read_min(a,b) ((a) < (b) ? (a) : (b))
    int len = read_min(128,maxlen);
    int ret = -1;
    int sum = 0;
    for(;;)
    {
        len = read_min(128,maxlen - sum);
        ret = ::read(fd,data,len);
        if(ret <= 0)
        {
            break;
        }
        sum += ret;
    }
    return sum;
}


uint64_t File::size() const
{
    return 0;
}
uint64_t File::pos() const
{
    return 0;
}
bool File::seek(uint64_t pos)
{
    return ::lseek(fd,pos,SEEK_SET) != -1; /*开头 */
}

bool File::atEnd()
{

}

bool File::flush()
{
    return false;
}

Permissons File::getPermisson() const
{

}
bool File::setPermisson(Permissons per)
{

}

uint8_t *File::map(uint64_t offset,uint64_t size)
{
    return (uint8_t *)::mmap(NULL,size,PROT_READ | PROT_WRITE,MAP_SHARED,fd,offset);
}

bool File::unmap(uint8_t *addr, uint64_t size)
{
    return ::munmap(addr,size);
}

