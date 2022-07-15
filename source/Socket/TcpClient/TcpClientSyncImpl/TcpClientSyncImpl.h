#ifndef TCPCLIENTSYNCIMPL_H_
#define TCPCLIENTSYNCIMPL_H_

#include "../../Session/Session.h"
#include "../TcpClient.h"

class TcpClientSyncImpl : public TcpClient {
public:
	TcpClientSyncImpl();
	virtual ~TcpClientSyncImpl();

	bool connect(const int& port, const std::string& ip = std::string()) final;
	bool disconnect() final;
	bool isConnected() const final;
	bool isRun() const final;
	std::string getServerIP() const final;
	std::uint16_t getPortNum() const final;
	bool bindHandler(const handler::Connect& handler) final;
	bool bindHandler(const handler::Msg& msg) final;
	std::string getConnectedInfo() const final;
	void setBufferSize(const std::size_t& size) final;
	std::size_t getBufferSize() const final;
	bool send(const std::vector<std::uint8_t>& send_msg) final;
	bool send(const std::uint8_t* data, const std::size_t& length) final;

private:
	Session::Ptr _session;

	asio::io_context _iocontext;

	std::string _dstIp;
	std::uint16_t _port;

	std::atomic_bool _isClientRun;
	std::atomic_bool _isClientForceStop;
	std::atomic_bool _isConnected;
	mutable std::mutex _sessionLock;

	handler::Msg _msgHandler;
	handler::Connect _connectHandler;

	std::thread _thConnecting;

	void _processConnecting();
};

#endif /* TCPCLIENTSYNCIMPL_H_ */