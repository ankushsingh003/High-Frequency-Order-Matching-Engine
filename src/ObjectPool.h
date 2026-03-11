#pragma once
#include <vector>
#include <cstdint>
#include <concepts>

template <std::destructible T>
class ObjectPool {
public:
    explicit ObjectPool(size_t size) : pool_(size), freeList_(size) {
        for (size_t i = 0; i < size; ++i) {
            freeList_[i] = &pool_[i];
        }
        freeCount_ = size;
    }

    template <typename... Args>
    T* acquire(Args&&... args) {
        if (freeCount_ == 0) return nullptr; // Pool exhausted
        T* obj = freeList_[--freeCount_];
        new (obj) T(std::forward<Args>(args)...); // Placement new
        return obj;
    }

    void release(T* obj) {
        obj->~T(); // Call destructor
        freeList_[freeCount_++] = obj;
    }

private:
    std::vector<T> pool_;
    std::vector<T*> freeList_;
    size_t freeCount_;
};
