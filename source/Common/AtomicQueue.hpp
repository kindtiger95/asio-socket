#ifndef ATOMICQUEUE_HPP_
#define ATOMICQUEUE_HPP_

#include <queue>
#include <mutex>

template<typename T>
class AtomicQueue
{
public:
	AtomicQueue() = default;
	AtomicQueue(const AtomicQueue<T>&) = delete;
	AtomicQueue& operator=(const AtomicQueue<T>&) = delete;
	AtomicQueue(AtomicQueue<T>&& other)
	{
		std::lock_guard<std::mutex> lock(this->_mutex);
		this->_queue = std::move(other._queue);
	}

	virtual ~AtomicQueue() = default;

	std::size_t size() const
	{
		std::lock_guard<std::mutex> lock(this->_mutex);
		return this->_queue.size();
	}

	T pop()
	{
		std::lock_guard<std::mutex> lock(this->_mutex);
		if (this->_queue.empty() == true)
			return {};
		T tmp = this->_queue.front();
		this->_queue.pop();
		return tmp;
	}

	void push(const T& t)
	{
		std::lock_guard<std::mutex> lock(this->_mutex);
		this->_queue.push(t);
	}

private:
	std::mutex _mutex;
	std::queue<T> _queue;
};

#endif /* LOCKFREEQUEUE_HPP_ */