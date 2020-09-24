#ifndef __TERROR_H
#define __TERROR_H

#include <string>

using std::string;

namespace ns_core
{

class TError
{
public:
    TError() {}
    TError(int code,string msg):mMsg(msg),mCode(code){}
    TError(string msg):mMsg(msg),mCode(-1) {}

    virtual string &getMessage() {return mMsg;}
    virtual int &getCode() {return mCode;}

private:
    string mMsg;
    int mCode;
};

}

#endif
