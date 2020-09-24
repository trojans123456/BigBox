#ifndef __IPC_FIFO_H
#define __IPC_FIFO_H

#include <string>

using std::string;

namespace ns_core
{

class ipc_fifo
{
public:
    ipc_fifo(string path);

    bool open();

private:

};


}


#endif
