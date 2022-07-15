#include "TcpServer.h"
#include "TcpServerSyncImpl/TcpServerSyncImpl.h"

TcpServer::Ptr TcpServer::createPtr(const param::SocketType& type)
{
	if (type == param::SocketType::Sync)
		return TcpServer::Ptr(new TcpServerSyncImpl());
	return nullptr;
}