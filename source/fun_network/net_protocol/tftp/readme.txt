/**
tftp (trivial file transfer protocol )简单文件传输协议 提供不复杂、开销不大的文件传输服务 端口号69，基于UDP
*/

1.传输模式

netascii        8bit的ascii码形式(文本模式)

octet           普通的二进制(二进制模式)

mail            过时不再使用

2.特点

    每次传输的数据报文有512字节。最后一次可以不足512字节
    数据报文按编号从1开始。

3.报文
-----------------------------------------------
|   local medium | internet | datagram | tftp |
------------------------------------------------
本地媒介头
ip头
数据报头
tftp头

4.请求类型  5种类型

4.1 读文件请求包(Read request RRQ Opcode = 1)

      2byte    string     1byte    string     1byte
    --------------------------------------------------
    | Opcode | Filename |   0   |   Mode    |   0    |
    --------------------------------------------------

    Mode = "netascii" or "octet" or "mail"

4.2 写文件请求包(Write request WRQ Opcode = 2)


4.3 文件数据包(Data DATA Opcode=3)

      2byte    2byte    nbyte
    --------------------------------
    | Opcode | Block # | Data |
    --------------------------------
    Block 从1开始
    Data 每次都为512字节。除了最后一次

4.4 回应包(Acknowledgement ACK Opcode = 4)

      2byte    2byte
    -------------------
    | Opcode | Block #|
    -------------------
    服务器或客户度发送ACK 回应收到的包

4.5 错误信息包(Error ERROR Opcode = 5)

       2byte    2byte       string  1byte
     ------------------------------------
     | Opcode | ErrorCode | ErrMsg |  0 |
     ------------------------------------
    0 未定义，请参阅错误信息（如果提示这种信息的话）

    1 文件未找到

    2 访问非法

    3 磁盘满或超过分配的配额

    4 非法的TFTP操作

    5 未知的传输ID 

    6 文件已经存在

    7 没有类似的用户

5 执行流程

    client                    server
      |                         |
      |       sendto            |
      |--------Write request--->| recvfrom
      |                         |
      |<---------回应-----------| sendto
      |                         |
      |--------ACK------------->| recvfrom
      |                         |
      |<---------Data-----------|sendto
      |                         |
      |------------ACK--------->|
