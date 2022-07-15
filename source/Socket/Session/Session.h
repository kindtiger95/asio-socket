#ifndef SESSION_H_
#define SESSION_H_

#include <thread>
#include <memory>
#include <vector>
#include <string>

#include <Common/AtomicQueue.hpp>
#include <Common/param.hpp>
#include <Common/handler.hpp>

class Session : public std::enable_shared_from_this<Session>
{
public:
	using Ptr = std::shared_ptr<Session>;
	
	static Ptr createPtr(param::BoostSocketPtr& boost_socket_ptr, const handler::Msg& msg_handler);

	Session() = delete;
	Session(const Session&) = delete;
	Session& operator=(const Session&) = delete;
	Session(param::BoostSocketPtr& boost_socket_ptr, const handler::Msg& msg_handler);
	Session(Session&& other);

	virtual ~Session();

	bool disconnect();

	std::string getIP() const;

	int getNativeHandle() const;

	bool send(const std::vector<std::uint8_t>& send_msg);

	bool send(const std::uint8_t* data, const std::size_t& length);

	bool getIsThreadEnd() const;

private:
	param::BoostSocketPtr _socket;
	AtomicQueue<param::Packet> _aQueue;
	handler::Msg _msgHandler;

	std::thread _thSession;
	std::atomic_bool _isThreadForceStop;
	std::atomic_bool _isThreadEnd;

	void _session();
};

#endif /* SESSION_H_ */