#pragma once

#include <unordered_map>
#include <thread>
#include <mutex>
#include <shared_mutex>



template<typename Kty, typename Ty, typename Hasher>
// Threadsafe map, not entirely comprehensive but it's enough
class SharedMap {
public:
	Ty& operator[](const Kty& p_key) {
		std::unique_lock<std::shared_mutex> lock(m_accessMutex);
		return m_map[p_key];
	}
	auto insert(std::pair<const Kty&, Ty> p_pair) {
		std::unique_lock<std::shared_mutex> lock(m_accessMutex);
		return m_map.insert(p_pair);
	}
	auto erase(const Kty& p_key) {
		std::unique_lock<std::shared_mutex> lock(m_accessMutex);
		return m_map.erase(p_key);
	}
	void reserve(size_t p_reserveAmount) {
		std::unique_lock<std::shared_mutex> lock(m_accessMutex);
		m_map.reserve(p_reserveAmount);
	}
	void clear() {
		std::unique_lock<std::shared_mutex> lock(m_accessMutex);
		m_map.clear();
	}
	auto find(const Kty& p_key) {
		std::shared_lock<std::shared_mutex> lock(m_accessMutex);
		return m_map.find(p_key);
	}
	bool contains(const Kty& p_key) {
		std::shared_lock<std::shared_mutex> lock(m_accessMutex);
		return m_map.contains(p_key);
	}
	auto begin() {
		std::shared_lock<std::shared_mutex> lock(m_accessMutex);
		return m_map.begin();
	}
	auto end() {
		std::shared_lock<std::shared_mutex> lock(m_accessMutex);
		return m_map.end();
	}
	auto size() {
		std::shared_lock<std::shared_mutex> lock(m_accessMutex);
		return m_map.size();
	}

	static SharedMap<Kty, Ty, Hasher>& Get() {
		static SharedMap<Kty, Ty, Hasher> instance = SharedMap<Kty, Ty, Hasher>();
		return instance;
	}
private:
	std::unordered_map<Kty, Ty, Hasher> m_map;
	std::shared_mutex m_accessMutex;
};