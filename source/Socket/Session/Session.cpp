#include <iostream>

#include "Session.h"

constexpr std::size_t INIT_BUFFER_SIZE = 1024;

Session::Ptr Session::createPtr(param::BoostSocketPtr& boost_socket_ptr, const handler::Msg& msg_handler)
{
	return Session::Ptr(new Session(boost_socket_ptr, msg_handler));
}
Session::Session(param::BoostSocketPtr& boost_socket_ptr, const handler::Msg& msg_handler) : _msgHandler(msg_handler), _isThreadEnd(false), _isThreadForceStop(false)
{
	this->_socket = std::move(boost_socket_ptr);
	this->_thSession = std::thread(&Session::_session, this);
}
Session::Session(Session&& other)
{
	this->_socket = std::move(other._socket);
	this->_msgHandler = other._msgHandler;
}
Session::~Session()
{
	this->disconnect();
}
bool Session::disconnect()
{
	this->_isThreadForceStop = true;
	try
	{
		this->_socket->shutdown(asio::ip::tcp::socket::shutdown_type::shutdown_both);
		this->_socket->close();
	}
	catch (...)
	{}

	if (this->_thSession.joinable() == true)
		this->_thSession.join();
	
	return true;
}
std::string Session::getIP() const
{
	std::string ret;
	try
	{
		ret = this->_socket->remote_endpoint().address().to_string();
	}
	catch (...)
	{
		return ret;
	}
	return ret;
}
int Session::getNativeHandle() const
{
	return this->_socket->native_handle();
}
bool Session::send(const std::vector<std::uint8_t>& send_msg)
{
	return send_msg.empty() ? false : send(send_msg.data(), send_msg.size());
}
bool Session::send(const std::uint8_t* data, const std::size_t& length)
{
	try
	{
		asio::write(*this->_socket, asio::buffer(data, length));
	}
	catch (...)
	{
		return false;
	}
	return true;
}
bool Session::getIsThreadEnd() const
{
	return this->_isThreadEnd;
}
/**************************************************** PRIVATE METHOD ****************************************************/
void Session::_session()
{
	std::cout << "[Session] Start the session [IP:" << this->getIP() << "]" << std::endl;
	this->_isThreadForceStop = false;
	while (this->_isThreadForceStop == false)
	{
		std::vector<std::uint8_t> buffer(INIT_BUFFER_SIZE);
		std::size_t length;
		try
		{
			length = this->_socket->read_some(asio::buffer(buffer));
		}
		catch (...)
		{
			break;
		}
		std::vector<std::uint8_t> packet(buffer.begin(), buffer.begin() + length);

		if (this->_msgHandler != nullptr)
			this->_msgHandler(packet);
	}
	this->_isThreadEnd = true;
}