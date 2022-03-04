#pragma once
#include "tcp_socket.h"
namespace scpp {
    class TcpServer :
        public TcpSocket
    {
        SOCKET accept();

        std::thread* listen_thread;
    public:

        bool init(unsigned short port);

        void on_connect(std::function<void(TcpSocket*)> callback);

        void stop_connect();
    };
}
