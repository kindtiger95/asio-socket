#include "TcpClient.h"
#include "TcpClientSyncImpl/TcpClientSyncImpl.h"

TcpClient::Ptr TcpClient::createPtr(const param::SocketType& type)
{
	if (type == param::SocketType::Sync)
		return TcpClient::Ptr(new TcpClientSyncImpl());
	return nullptr;
}