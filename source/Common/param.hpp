#ifndef PARAM_H_
#define PARAM_H_

#include <asio.hpp>
#include <vector>

namespace param
{
	using BoostSocket = asio::ip::tcp::socket;
	using BoostSocketPtr = std::shared_ptr<BoostSocket>;
	using Packet = std::vector<std::uint8_t>;
	enum class SocketType { Async, Sync };
	constexpr int NOT_SET_VALUE = -1;
}

#endif /* PARAM_H_ */