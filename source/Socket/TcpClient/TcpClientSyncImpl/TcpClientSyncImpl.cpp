#include "TcpClientSyncImpl.h"

TcpClientSyncImpl::TcpClientSyncImpl() 
	: _port(param::NOT_SET_VALUE), _isClientRun(false), _isClientForceStop(false), _isConnected(false)
{}
TcpClientSyncImpl::~TcpClientSyncImpl() {
	this->disconnect();
}
bool TcpClientSyncImpl::connect(const int& port, const std::string& ip) {
	if (this->_isConnected == true) {
		std::cout << "[TCPClient] Already connected client socket to server.";
		return false;
	}

	if (port < 0 || port > 65535)
	{
		std::cout << "[TCPClient] Port number is not available. " << std::endl;
		return false;
	}

	if (this->_msgHandler == nullptr)
	{
		std::cout << "[TCPClient] Binding the Msg Handler first." << std::endl;
		return false;
	}

	if (this->_isClientRun == true)
	{
		std::cout << "[TCPClient] Connecting Thread is already exist." << std::endl;
		return false;
	}

	this->_port = port;
	this->_dstIp = ip;
	this->_isClientForceStop = false;
	this->_thConnecting = std::thread(&TcpClientSyncImpl::_processConnecting, this);

	return true;
}
bool TcpClientSyncImpl::disconnect() {
	std::cout << "[TCPClient] Client disconnecting is called..." << std::endl;
	
	this->_isClientForceStop = true;
	
	/* Waiting connecting thread finished */
	if (this->_thConnecting.joinable() == true)
		this->_thConnecting.join();

	/* socket is close */
	this->_sessionLock.lock();
	this->_session.reset();
	this->_sessionLock.unlock();

	std::this_thread::sleep_for(std::chrono::milliseconds(200));
	std::cout << "[TCPClient] Success disconnect. " << std::endl;
	return true;
}
bool TcpClientSyncImpl::isConnected() const {
	return this->_isConnected;
}
bool TcpClientSyncImpl::isRun() const {
	return this->_isClientRun;
}
std::string TcpClientSyncImpl::getServerIP() const {
	return this->_dstIp;
}
std::uint16_t TcpClientSyncImpl::getPortNum() const {
	return this->_port;
}
bool TcpClientSyncImpl::bindHandler(const handler::Connect& handler) {
	if (handler != nullptr)
		this->_connectHandler = handler;
	return true;
}
bool TcpClientSyncImpl::bindHandler(const handler::Msg& msg) {
	if (msg != nullptr)
		this->_msgHandler = msg;
	return true;
}
std::string TcpClientSyncImpl::getConnectedInfo() const {
	return std::string();
}
void TcpClientSyncImpl::setBufferSize(const std::size_t& size) {

}
std::size_t TcpClientSyncImpl::getBufferSize() const {
	return std::size_t();
}
bool TcpClientSyncImpl::send(const std::vector<std::uint8_t>& send_msg) {
	std::lock_guard<std::mutex> guard(this->_sessionLock);
	this->_session->send(send_msg);
	return true;
}
bool TcpClientSyncImpl::send(const std::uint8_t* data, const std::size_t& length) {
	std::lock_guard<std::mutex> guard(this->_sessionLock);
	this->_session->send(data, length);
	return true;
}
/**************************************************** PRIVATE METHOD ****************************************************/
void TcpClientSyncImpl::_processConnecting() {
	this->_isClientRun = true;
	asio::ip::tcp::endpoint ep(asio::ip::address::from_string(this->_dstIp), this->_port);
	while (this->_isClientForceStop == false) {
		if (this->_session != nullptr) {
			if (this->_session->getIsThreadEnd() == false) {
				std::this_thread::sleep_for(std::chrono::seconds(1));
				continue;
			}
			else {
				this->_sessionLock.lock();
				this->_session.reset();
				this->_sessionLock.unlock();
			}
		}
		param::BoostSocketPtr socket_ptr = std::make_shared<asio::ip::tcp::socket>(this->_iocontext);

		try {
			std::cout << "[TCPClient] Try connect to server ... [IP " + std::string(this->_dstIp) + ":" + std::to_string(static_cast<int>(this->_port)) + "]" << std::endl;
			socket_ptr->connect(ep);
		}
		catch (...) {
			std::this_thread::sleep_for(std::chrono::milliseconds(200));
			continue;
		}

		this->_sessionLock.lock();
		this->_session = Session::createPtr(socket_ptr, this->_msgHandler);
		this->_sessionLock.unlock();
	}
	this->_isClientRun = false;
}