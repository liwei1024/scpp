#include "scpp.h"
#include "tcp_server.h"

namespace scpp {
	SOCKET TcpServer::accept()
	{
		int len = sizeof(SOCKADDR);
		SOCKADDR_IN address;
		return ::accept(socket, (SOCKADDR*)&address, &len);
	}


	bool TcpServer::init(unsigned short port)
	{
		SOCKADDR_IN address = {};

		address.sin_addr.S_un.S_addr = htonl(INADDR_ANY);//INADDR_ANY表示任何IP
		address.sin_family = AF_INET;
		address.sin_port = htons(port);//绑定端口6000

		socket = ::socket(AF_INET, SOCK_STREAM, 0);

		if (socket == INVALID_SOCKET)
		{
			return false;
		}

		if (bind(socket, (SOCKADDR*)&address, sizeof(SOCKADDR)) == SOCKET_ERROR)
		{
			return false;
		}

		if (listen(socket, SOMAXCONN) == SOCKET_ERROR)
		{
			return false;
		}

		return true;
	}

	void TcpServer::on_connect(std::function<void(TcpSocket*)> callback)
	{
		listen_thread = new std::thread(
			[this, callback]() {
				while (true)
				{
					TcpSocket* tcp_socket = new TcpSocket();

					tcp_socket->socket = this->accept();

					if (tcp_socket->socket == SOCKET_ERROR)
					{
						delete tcp_socket;
						break;
					}

					callback(tcp_socket);
				}
			}
		);
	}

	void TcpServer::stop_connect()
	{
		this->close();

		if (listen_thread)
		{
			listen_thread->join();
			delete listen_thread;
			listen_thread = nullptr;
		}
	}
}