#pragma once
#include <queue>
#include <mutex>
#include <shared_mutex>
#include <condition_variable>
#include <optional>

template<typename T>
class SharedQueue {
public:

	void push(const T& p_value) {
		std::unique_lock<std::shared_mutex> lock(m_mutex);
		m_queue.push(p_value);
		m_cv.notify_one();
	}
	auto pop() {
		std::unique_lock<std::shared_mutex> lock(m_mutex);
		m_cv.wait(lock, [this]() { return !m_queue.empty(); });
		T out = m_queue.front();
		m_queue.pop();
		return out;
	}
	bool empty() {
		std::shared_lock<std::shared_mutex> lock(m_mutex);
		return m_queue.empty();
	}
	std::optional<T> tryPop() {
		if (!empty()) {
			T out = m_queue.front();
			m_queue.pop();
			return out;
		}
		return std::nullopt;
	}
private:
	std::queue<T> m_queue;
	std::shared_mutex m_mutex;
	std::condition_variable m_cv;
};