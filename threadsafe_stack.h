#include <exception>
#include <memory>
#include <mutex>
#include <stack>

struct empty_stack : std::exception {
	const char* what() const throw();
}

template<typename T>
class threadsafe_stack {
public:
	threadsafe_stack();
	threadsafe_stack(const threadsafe_stack&);
	threadsafe_stack& operator=(const threadsafe_stack&) = delete;

	void push(T new_value);

	std::shared_ptr<T> pop();
	void pop(T& value);
	
	bool empty() const;
};

template<typename T>
class threadsafe_stack {
public:
	threadsafe_stack(){}
	threadsafe_stack(const threadsafe_stack& other) {
		std::lock_guard<std::mutex> lock(_M_mutex)
		_M_data = other._M_data;
	}

	threadsafe_stack& operator=(const threadsafe_stack&) = delete;

	void push(T new_value) {
		std::lock_guard<std::mutex> lock(_M_mutex);
		_M_data.push(new_value);
	}

	std::shared_ptr<T> pop() {
		std::lock_guard<std::mutex> lock(_M_mutex);
		if(_M_data.empty())
			throw empty_stack();

		const std::shared_ptr<T> res(std::make_shared<T>(_M_data.top()));
		_M_data.pop();

		return res;
	}

	//pop 出引用
	void pop(T& value) {
		std::lock_guard<std::mutex> lock(_M_mutex);
		if(_M_data.empty())
			throw empty_stack();

		value = _M_data.top();
		_M_data.pop();
	}

	bool empty() const {
		std::lock_guard<std::mutex> lock(_M_mutex);
		return _M_data.empty();
	}

private:
	std::stack<T> _M_data;
	mutable std::mutex    _M_mutex
}

















