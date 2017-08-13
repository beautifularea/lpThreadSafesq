#include <memory>
#include <mutex>
#include <condition_variable>

template<typename T>
class threadsafe_queue {
public:
	threadsafe_queue() {}

	threadsafe_queue(const threadsafe_queue& other) {
		std::lock_guard<std::mutex> lk(other._M_mutex);
		_M_queue = other._M_queue;
	}

	threadsafe_queue& operator=(const threadsafe_queue&) = delete;

	void push(T new_value) {
		std::lock_guard<std::mutex> lk(_M_mutex);
		_M_queue.push(new_value);
		_M_condition.notify_one();
	}

	bool try_pop(T& value) {
		std::lock_guard<std::mutex> lk(_M_mutex);
		if(_M_queue.empty()) return false;
		value = _M_queue.front();
		_M_queue.pop();

		return true;
	}

	std::shared_ptr<T> try_pop() {
		std::lock_guard<std::mutex> lk(_M_mutex);
		if(_M_queue.empty()) return std::shared_ptr<T>();
		std::shared_ptr<T> res(std::make_shared<T>(_M_queue.front()));
		_M_queue.pop();

		return res;
	}

	void wait_and_pop(T& value) {
		std::unique_lock<std::mutex> lk(_M_mutex);
		_M_condition.wait(lk, (this){ return !_M_queue.empty(); })

		value = _M_queue.front();
		_M_queue.pop();
	}

	std::shared_ptr<T> wait_and_pop() {
		std::unique_lock<std::mutex> lk(_M_mutex);
		_M_condition.wait(lk, [this]{ return !_M_queue.empty(); });
		std::shared_ptr<T> res(std::make_shared<T>(_M_queue.front()));
		_M_queue.pop();

		return res;
	}

	bool empty() const {
		std::lock_guard<std::mutex> lk(_M_mutex);
		return _M_queue.empty();
	}

private:
	std::mutex _M_mutex;
	std::queue<T> _M_queue;
	std::condition_variable _M_condition;
};