#pragma once
#include "tcp_socket.h"
namespace scpp {
    class TcpClient :
        public TcpSocket
    {
    public:
        bool connect(const char* ip, unsigned short port);
    };
}
