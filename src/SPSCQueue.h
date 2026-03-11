#pragma once
#include <atomic>
#include <vector>
#include <cassert>
#include <concepts>

template <std::default_initializable T>
class SPSCQueue {
public:
    explicit SPSCQueue(size_t capacity) 
        : capacity_(capacity), buffer_(capacity) {
        
        
    }

    bool push(const T& item) {
        auto current_tail = tail_.load(std::memory_order_relaxed);
        auto next_tail = (current_tail + 1) % capacity_;
        
        if (next_tail == head_.load(std::memory_order_acquire)) {
            return false; 
        }

        buffer_[current_tail] = item;
        tail_.store(next_tail, std::memory_order_release);
        return true;
    }

    bool pop(T& item) {
        auto current_head = head_.load(std::memory_order_relaxed);
        
        if (current_head == tail_.load(std::memory_order_acquire)) {
            return false; 
        }

        item = buffer_[current_head];
        head_.store((current_head + 1) % capacity_, std::memory_order_release);
        return true;
    }

private:
    size_t capacity_;
    std::vector<T> buffer_;
    
    
    alignas(64) std::atomic<size_t> head_{0};
    alignas(64) std::atomic<size_t> tail_{0};
};
