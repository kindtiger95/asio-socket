#ifndef TCPCLIENT_H_
#define TCPCLIENT_H_

#include <memory>
#include <string>

#include <common/param.hpp>
#include <common/handler.hpp>

class TcpClient
{
public:
	using Ptr = std::unique_ptr<TcpClient>;

	static Ptr createPtr(const param::SocketType& type);

	virtual ~TcpClient() = default;

	virtual bool connect(const int& port, const std::string& ip = std::string()) = 0;

	virtual bool disconnect() = 0;

	virtual bool isConnected() const = 0;

	/* connect를 시도하고 있는지 반환 */
	virtual bool isRun() const = 0;

	/* 클라이언트일 경우 접속을 시도하고 있는 서버의 IP를 반환 */
	virtual std::string getServerIP() const = 0;

	virtual std::uint16_t getPortNum() const = 0;

	/* 접속 상태를 확인하여 처리할 핸들러 */
	virtual bool bindHandler(const handler::Connect& handler) = 0;

	/* 받은 메세지를 처리할 핸들러 */
	virtual bool bindHandler(const handler::Msg& msg) = 0;

	/* 접속된 서버의 IP를 반환 */
	virtual std::string getConnectedInfo() const = 0;

	virtual void setBufferSize(const std::size_t& size) = 0;

	virtual std::size_t getBufferSize() const = 0;

	virtual bool send(const std::vector<std::uint8_t>& send_msg) = 0;

	virtual bool send(const std::uint8_t* data, const std::size_t& length) = 0;
};
#endif /* TCPCLIENT_H_ */