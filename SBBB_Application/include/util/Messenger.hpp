#include <thread>
#include <chrono>
#include <queue>
#include <mutex>
#include <optional>
#include <string>

template<typename TFront, typename TBack>
// The messenger class handles thread synnchronization in a non-blocking way, allowing the sharing of resources
// The front and back in this context refer to forward and reverse directions of messaging, allowing two way communication.
class Messenger {
public:
    Messenger() {}

    std::optional<TFront> getMessageFront() {
        std::unique_lock<std::mutex> lock(m_frontMutex);
        if (m_frontQueue.empty()) return std::nullopt;
        const TFront out = m_frontQueue.front();
        m_frontQueue.pop();
        return out;
    }

    std::optional<TBack> getMessageBack() {
        std::unique_lock<std::mutex> lock(m_backMutex);
        if (m_backQueue.empty()) return std::nullopt;
        const TBack out = m_backQueue.front();
        m_backQueue.pop();
        return out;
    }
    // value overloads
    void sendMessageFront(TFront p_message) {
        std::unique_lock<std::mutex> lock(m_frontMutex);
        m_frontQueue.push(p_message);
    }

    void sendMessageBack(TBack p_message) {
        std::unique_lock<std::mutex> lock(m_backMutex);
        m_backQueue.push(p_message);
    }
    // reference overloads for less copying, non-const
    void sendMessageFront(TFront& p_message) {
        std::unique_lock<std::mutex> lock(m_frontMutex);
        m_frontQueue.push(p_message);
    }

    void sendMessageBack(TBack& p_message) {
        std::unique_lock<std::mutex> lock(m_backMutex);
        m_backQueue.push(p_message);
    }

private:
    std::queue<TFront> m_frontQueue;
    std::queue<TBack> m_backQueue;

    std::mutex m_frontMutex;
    std::mutex m_backMutex;
};