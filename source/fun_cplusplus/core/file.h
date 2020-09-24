#ifndef __FILE_H
#define __FILE_H

#include <stdio.h>
#include <stdint.h>

#include <string>
#include <fstream>

using std::string;


namespace ns_core {

typedef enum Permisson
{
    ReadOwner = 0x4000, WriteOwner = 0x2000, ExeOwner = 0x1000,
    ReadUser  = 0x0400, WriteUser  = 0x0200, ExeUser  = 0x0100,
    ReadGroup = 0x0040, WriteGroup = 0x0020, ExeGroup = 0x0010,
    ReadOther = 0x0004, WriteOther = 0x0002, ExeOther = 0x0001
}Permissons;

enum OpenModeFlag
{
    NotOpen = 0x0000,
    ReadOnly = 0x0001,
    WriteOnly = 0x0002,
    ReadWrite = ReadOnly | WriteOnly,
    Append = 0x0004
};
typedef enum OpenModeFlag OpenMode;

class File
{
public:


    OpenMode getOpenMode() const;
    void setOpenMode(OpenMode openMode);

    bool isOpen() const;
    bool isReadable() const;
    bool isWriteable() const;

    File();
    File(const string &name);
    ~File();

    string fileName() const;
    void setFileName(const string &name);

    bool exists() const;
    static bool exists(const string &filename);

    string readLink() const;
    static string readLink(const string &filename);

    bool remove();
    static bool remove(const string &filename);

    bool rename(const string &newName);
    static bool rename(const string &oldName,const string &newName);

    bool link(const string &newName);
    static bool link(const string &oldName,const string &newName);

    bool open(OpenMode flags);
    bool open(FILE *f,OpenMode flags);
    bool open(int fd,OpenMode flags);

    virtual void close();

    uint64_t read(char *data,uint64_t maxlen);
    uint64_t write(char *data,uint64_t maxlen);

    uint64_t readLine(char *data,uint64_t maxlen);


    uint64_t size() const;
    uint64_t pos() const;
    bool seek(uint64_t pos);
    bool atEnd();
    bool flush();

    Permissons getPermisson() const;
    bool setPermisson(Permissons per);

    uint8_t *map(uint64_t offset,uint64_t size);
    bool unmap(uint8_t *addr,uint64_t size);


private:
    string mFileName;
    OpenMode mOpenMode;
    int fd;
};

}

#endif
