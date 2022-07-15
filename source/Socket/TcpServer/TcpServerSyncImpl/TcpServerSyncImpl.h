#ifndef TCPSERVERSYNCIMPL_H_
#define TCPSERVERSYNCIMPL_H_

#include "../../Session/Session.h"
#include "../TcpServer.h"

#include <atomic>
#include <thread>
#include <string>
#include <vector>
#include <map>

class TcpServerSyncImpl : public TcpServer
{
public:
	TcpServerSyncImpl();
	virtual ~TcpServerSyncImpl();

	bool connect(const int& port, const std::string& ip = std::string()) final;
	bool disconnect() final;
	std::size_t getConnectedCount() const final;
	bool isRun() const final;
	std::string getServerIP() const final;
	std::uint16_t getPortNum() const final;
	bool bindHandler(const handler::Connect& handler) final;
	bool bindHandler(const handler::Msg& handler) final;
	std::vector<std::string> getConnectedInfo() const final;
	void setBufferSize(const std::size_t& size) final;
	std::size_t getBufferSize() const final;
	bool send(const std::vector<std::uint8_t>& send_msg) final;
	bool send(const std::uint8_t* data, const std::size_t& length) final;

private:
	std::map<int, Session::Ptr> _sessionMap;

	asio::io_context _iocontext;
	asio::ip::tcp::acceptor _acceptor;

	std::uint16_t _port;

	std::atomic_bool _isServerRun;
	std::atomic_bool _isServerForceStop;
	std::atomic_bool _isObserverRun;
	std::atomic_bool _isObserverForceStop;
	mutable std::mutex _sessionLock;

	handler::Msg _msgHandler;
	handler::Connect _connectHandler;

	std::thread _thAccept;
	std::thread _thObserve;

	void _processAccept();
	void _processObserve();
};
#endif /* TCPSERVERSYNCIMPL_H_ */