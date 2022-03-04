#pragma once


#pragma comment(lib, "Ws2_32")
namespace scpp {
	class TcpSocket
	{
	public:
		SOCKET socket;
	public:
		TcpSocket();
		~TcpSocket();

		void close();

		bool setsockopt(int level, int optname, void* optval, int optlen);

		bool set_sndbuf_len(int send_buf_len);

		bool set_rcvbuf_len(int recv_buf_len);

		bool set_tcp_nodelay(bool enable);

		bool send(void* buffer, size_t size);

		bool recv(void* buffer, size_t size);
	};
}

