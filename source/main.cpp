#include <iostream>

#include "Socket/TcpServer/TcpServer.h"
#include "Socket/TcpClient/TcpClient.h"

bool testBinding(const param::Packet& msg)
{
	for (const auto& it : msg)
		std::cout << it;
	std::cout << std::endl;
	return true;
}

void serverSocketTest() {
	const auto ServerSocket = TcpServer::createPtr(param::SocketType::Sync);
	ServerSocket->bindHandler(std::bind(testBinding, std::placeholders::_1));
	ServerSocket->connect(9300);

	while (true)
	{
		std::uint8_t command_number;
		std::cin >> command_number;
		std::cout << "Input command number: " << command_number << std::endl;
		if (command_number == '1') {
			ServerSocket->disconnect();
		}
		else if (command_number == '2')
			ServerSocket->connect(9300);
	}
}

void clientSocketTest() {
	const auto ClientSocket = TcpClient::createPtr(param::SocketType::Sync);
	ClientSocket->bindHandler(std::bind(testBinding, std::placeholders::_1));
	ClientSocket->connect(9300, std::string("127.0.0.1"));

	while (true)
	{
		std::uint8_t command_number;
		std::cin >> command_number;
		std::cout << "Input command number: " << command_number << std::endl;
		if (command_number == '1') {
			ClientSocket->disconnect();
		}
		else if (command_number == '2')
			ClientSocket->connect(9300, std::string("127.0.0.1"));
	}
}

int main()
{
	clientSocketTest();
	return 0;
}