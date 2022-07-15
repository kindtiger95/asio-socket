#ifndef TCPSERVER_H_
#define TCPSERVER_H_

#include <memory>
#include <string>

#include <common/param.hpp>
#include <common/handler.hpp>

class TcpServer
{
public:
	using Ptr = std::unique_ptr<TcpServer>;

	static Ptr createPtr(const param::SocketType& type);

	virtual ~TcpServer() = default;

	virtual bool connect(const int& port, const std::string& ip = std::string()) = 0;

	virtual bool disconnect() = 0;

	virtual std::size_t getConnectedCount() const = 0;

	/* acceptor가 동작하고 있는지 반환 */
	virtual bool isRun() const = 0;

	/* 자신의 IP를 반환 */
	virtual std::string getServerIP() const = 0;

	virtual std::uint16_t getPortNum() const = 0;

	/* 접속 상태를 확인하여 처리할 핸들러 */
	virtual bool bindHandler(const handler::Connect& handler) = 0;

	/* 받은 메세지를 처리할 핸들러 */
	virtual bool bindHandler(const handler::Msg& msg) = 0;

	/* 현재 접속해있는 모든 클라이언트들의 IP */
	virtual std::vector<std::string> getConnectedInfo() const = 0;

	virtual void setBufferSize(const std::size_t& size) = 0;

	virtual std::size_t getBufferSize() const = 0;
	
	virtual bool send(const std::vector<std::uint8_t>& send_msg) = 0;

	virtual bool send(const std::uint8_t* data, const std::size_t& length) = 0;
};

#endif /* TCPSERVER_H_ */