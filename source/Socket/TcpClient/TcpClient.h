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

	/* connect�� �õ��ϰ� �ִ��� ��ȯ */
	virtual bool isRun() const = 0;

	/* Ŭ���̾�Ʈ�� ��� ������ �õ��ϰ� �ִ� ������ IP�� ��ȯ */
	virtual std::string getServerIP() const = 0;

	virtual std::uint16_t getPortNum() const = 0;

	/* ���� ���¸� Ȯ���Ͽ� ó���� �ڵ鷯 */
	virtual bool bindHandler(const handler::Connect& handler) = 0;

	/* ���� �޼����� ó���� �ڵ鷯 */
	virtual bool bindHandler(const handler::Msg& msg) = 0;

	/* ���ӵ� ������ IP�� ��ȯ */
	virtual std::string getConnectedInfo() const = 0;

	virtual void setBufferSize(const std::size_t& size) = 0;

	virtual std::size_t getBufferSize() const = 0;

	virtual bool send(const std::vector<std::uint8_t>& send_msg) = 0;

	virtual bool send(const std::uint8_t* data, const std::size_t& length) = 0;
};
#endif /* TCPCLIENT_H_ */