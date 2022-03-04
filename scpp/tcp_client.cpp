#include "scpp.h"
#include "tcp_client.h"

namespace scpp {

	bool TcpClient::connect(const char* ip, unsigned short port)
	{
		SOCKADDR_IN address = {};

		address.sin_family = AF_INET;
		address.sin_addr.s_addr = inet_addr(ip);//
		address.sin_port = htons(port);

		socket = ::socket(AF_INET, SOCK_STREAM, 0);

		if (socket == INVALID_SOCKET)
			return false;

		if (!set_tcp_nodelay(true))
			return false;

		if (::connect(socket, (SOCKADDR*)&address, sizeof(address)) == SOCKET_ERROR)
			return false;

		return true;
	}
}