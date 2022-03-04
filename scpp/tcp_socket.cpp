#include "scpp.h"
#include "tcp_socket.h"

namespace scpp {
    TcpSocket::TcpSocket()
    {
        WSADATA wsa_data;
        WSAStartup(MAKEWORD(2, 2), &wsa_data);

        socket = 0;
    }

    TcpSocket::~TcpSocket()
    {
        if (socket)
            ::closesocket(socket);

    }

    void TcpSocket::close()
    {
        if (socket)
            ::closesocket(socket);
    }

    bool TcpSocket::setsockopt(int level, int optname, void* optval, int optlen)
    {
        return ::setsockopt(socket, level, optname, (const char*)optval, optlen) != SOCKET_ERROR;
    }

    bool TcpSocket::set_sndbuf_len(int send_buf_len)
    {
        return setsockopt(SOL_SOCKET, SO_SNDBUF, &send_buf_len, sizeof(send_buf_len));
    }

    bool TcpSocket::set_rcvbuf_len(int recv_buf_len)
    {
        return setsockopt(SOL_SOCKET, SO_RCVBUF, &recv_buf_len, sizeof(recv_buf_len));
    }

    bool TcpSocket::set_tcp_nodelay(bool enable)
    {
        return setsockopt(IPPROTO_TCP, TCP_NODELAY, &enable, sizeof(enable));
    }

    bool TcpSocket::send(void* buffer, size_t len)
    {
        //printf("send buffer %p len %lld\n", buffer, len);
        int send_len = ::send(socket, (const char*)buffer, (int)len, 0);
        if (send_len == len)
            return true;

        if (send_len <= 0)
            return false;

        return this->send((char*)buffer + send_len, len - send_len);
    }

    bool TcpSocket::recv(void* buffer, size_t len)
    {
        //printf("recv buffer %p len %lld\n", buffer, len);
        int recv_len = ::recv(socket, (char*)buffer, (int)len, 0);
        if (recv_len == len)
            return true;

        if (recv_len <= 0)
            return false;

        return this->recv((char*)buffer + recv_len, len - recv_len);
    }
}