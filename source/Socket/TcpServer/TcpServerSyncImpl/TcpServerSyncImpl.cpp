#include <iostream>
#include <chrono>

#include <Common/param.hpp>

#include "TcpServerSyncImpl.h"

constexpr int OBSERVER_WAIT_TIME = 1;

TcpServerSyncImpl::TcpServerSyncImpl()
	: _acceptor(this->_iocontext), _port(param::NOT_SET_VALUE),
	_isServerRun(false), _isServerForceStop(false), _isObserverRun(false), _isObserverForceStop(false)
{
}
TcpServerSyncImpl::~TcpServerSyncImpl()
{
	this->disconnect();
}
bool TcpServerSyncImpl::connect(const int& port, const std::string& ip)
{
	this->disconnect();

	if (port < 0 || port > 65535)
	{
		std::cout << "[TCPServer] Port number is not available. " << std::endl;
		return false;
	}

	if (this->_msgHandler == nullptr)
	{
		std::cout << "[TCPServer] Binding the Msg Handler first." << std::endl;
		return false;
	}

	if (this->_isServerRun == true || this->_isObserverRun == true)
	{
		std::cout << "[TCPServer] Server is already exist." << std::endl;
		return false;
	}

	try
	{
		asio::ip::tcp::endpoint ep(asio::ip::tcp::v4(), port);
		this->_acceptor.open(ep.protocol());
		this->_acceptor.set_option(asio::ip::tcp::acceptor::reuse_address(true));
		this->_acceptor.bind(ep);
		this->_acceptor.listen();
	}
	catch (...)
	{
		std::cout << "[TCPServer] Port[" << port << "] is already using now." << std::endl;
		return false;
	}

	this->_isObserverForceStop = false;
	this->_isServerForceStop = false;

	this->_port = port;
	// thread is init
	this->_thAccept = std::thread(&TcpServerSyncImpl::_processAccept, this);
	this->_thObserve = std::thread(&TcpServerSyncImpl::_processObserve, this);

	return true;
}
bool TcpServerSyncImpl::disconnect()
{
	std::cout << "[TCPServer] Server disconnecting is called..." << std::endl;
	if (this->_isServerRun == false || this->_isObserverRun == false)
	{
		std::cout << "[TCPServer] Server is not running." << std::endl;
		return true;
	}

	this->_isServerForceStop = true;

	/* Waiting observer thread finished */
	if (this->_thObserve.joinable() == true)
		this->_thObserve.join();

	/* Break the blocking the acceptor */
	try
	{
		this->_acceptor.cancel();
		this->_acceptor.close();
		asio::ip::tcp::socket temp_socket(this->_iocontext);
		temp_socket.connect(asio::ip::tcp::endpoint(asio::ip::address::from_string("127.0.0.1"), this->_port));
	}
	catch (...) {}

	/* Waiting server thread finished */
	if (this->_thAccept.joinable() == true)
		this->_thAccept.join();

	/* All socket is close */
	this->_sessionLock.lock();
	this->_sessionMap.clear();
	this->_sessionLock.unlock();

	this->_isObserverForceStop = true;
	std::this_thread::sleep_for(std::chrono::seconds(OBSERVER_WAIT_TIME));

	if (this->_thObserve.joinable() == true)
		this->_thObserve.join();

	std::cout << "[TCPServer] Success disconnect. " << std::endl;
	return true;
}
std::size_t TcpServerSyncImpl::getConnectedCount() const
{
	std::lock_guard<std::mutex> guard(this->_sessionLock);
	return this->_sessionMap.size();
}
bool TcpServerSyncImpl::isRun() const
{
	return true;
}
std::string TcpServerSyncImpl::getServerIP() const
{
	return std::string();
}
std::uint16_t TcpServerSyncImpl::getPortNum() const
{
	std::uint16_t ret = 0;
	return ret;
}
bool TcpServerSyncImpl::bindHandler(const handler::Connect& handler)
{
	this->_connectHandler = handler;
	return true;
}
bool TcpServerSyncImpl::bindHandler(const handler::Msg& handler)
{
	this->_msgHandler = handler;
	return true;
}
std::vector<std::string> TcpServerSyncImpl::getConnectedInfo() const
{
	std::vector<std::string> ret;
	return ret;
}
void TcpServerSyncImpl::setBufferSize(const std::size_t& size)
{

}
std::size_t TcpServerSyncImpl::getBufferSize() const
{
	std::size_t ret = 0;
	return ret;
}
bool TcpServerSyncImpl::send(const std::vector<std::uint8_t>& send_msg)
{
	std::lock_guard<std::mutex> guard(this->_sessionLock);
	for (const auto& session_map_it : this->_sessionMap)
	{
		if (session_map_it.second->getIsThreadEnd() == false)
			session_map_it.second->send(send_msg);
	}
	return true;
}
bool TcpServerSyncImpl::send(const std::uint8_t* data, const std::size_t& length)
{
	std::lock_guard<std::mutex> guard(this->_sessionLock);
	for (const auto& session_map_it : this->_sessionMap)
	{
		if (session_map_it.second->getIsThreadEnd() == false)
			session_map_it.second->send(data, length);
	}
	return true;
}
/**************************************************** PRIVATE METHOD ****************************************************/
void TcpServerSyncImpl::_processAccept()
{
	this->_isServerRun = true;
	this->_isServerForceStop = false;

	std::cout << "[TCPServer] Open server [PORT:" << this->_port << "], listen client's request ... " << std::endl;
	while (this->_isServerForceStop == false)
	{
		param::BoostSocketPtr socket_ptr = std::make_shared<asio::ip::tcp::socket>(this->_iocontext);

		try
		{
			*socket_ptr = this->_acceptor.accept();
		}
		catch (const asio::error_code& ec) { break; }

		const int socket_native_handle = socket_ptr->native_handle();
		std::lock_guard<std::mutex> guard(this->_sessionLock);

		const auto& session_map = this->_sessionMap.find(socket_native_handle);

		if (session_map != this->_sessionMap.end())
		{
			std::cout << "[TCPServer] The client is already connected. Try reconnecting after a while. " << std::endl;
			continue;
		}
		this->_sessionMap[socket_native_handle] = Session::createPtr(socket_ptr, this->_msgHandler);
	}
	this->_isServerRun = false;
}
void TcpServerSyncImpl::_processObserve()
{
	this->_isObserverRun = true;
	this->_isObserverForceStop = false;
	std::cout << "[TCPServer] Start the observer thread ... " << std::endl;
	while (this->_isServerForceStop == false)
	{
		std::this_thread::sleep_for(std::chrono::seconds(OBSERVER_WAIT_TIME));
		std::lock_guard<std::mutex> guard(this->_sessionLock);
		std::vector<int> erase_list;
		for (const auto& session_map_it : this->_sessionMap)
		{
			if (session_map_it.second->getIsThreadEnd() == true)
				erase_list.push_back(session_map_it.second->getNativeHandle());
		}

		for (const auto& it : erase_list)
		{
			if (this->_sessionMap.find(it) != this->_sessionMap.end())
				this->_sessionMap.erase(it);
		}
	}
	this->_isObserverRun = false;
}