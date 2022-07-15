#ifndef HANDLER_HPP_
#define HANDLER_HPP_

#include <vector>
#include <functional>
#include <iostream>

#include "param.hpp"

namespace handler
{
	using Msg = std::function<bool(const param::Packet& msg)>;
	using Connect = std::function<bool(const bool& is_connected)>;
}	

#endif /* HANDLER_HPP_ */