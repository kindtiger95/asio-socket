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

	/* acceptor�� �����ϰ� �ִ��� ��ȯ */
	virtual bool isRun() const = 0;

	/* �ڽ��� IP�� ��ȯ */
	virtual std::string getServerIP() const = 0;

	virtual std::uint16_t getPortNum() const = 0;

	/* ���� ���¸� Ȯ���Ͽ� ó���� �ڵ鷯 */
	virtual bool bindHandler(const handler::Connect& handler) = 0;

	/* ���� �޼����� ó���� �ڵ鷯 */
	virtual bool bindHandler(const handler::Msg& msg) = 0;

	/* ���� �������ִ� ��� Ŭ���̾�Ʈ���� IP */
	virtual std::vector<std::string> getConnectedInfo() const = 0;

	virtual void setBufferSize(const std::size_t& size) = 0;

	virtual std::size_t getBufferSize() const = 0;
	
	virtual bool send(const std::vector<std::uint8_t>& send_msg) = 0;

	virtual bool send(const std::uint8_t* data, const std::size_t& length) = 0;
};

#endif /* TCPSERVER_H_ */