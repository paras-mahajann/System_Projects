#pragma once
#include <cassert>
#include <cstddef>
#include <utility>
#include <vector>

template <typename T, size_t PoolSize>
class PoolAllocator {
    union Slot {
        alignas(T) std::byte storage[sizeof(T)];
        Slot* next;
    };

    std::vector<Slot> pool;
    Slot* freeListHead;

   public:
    PoolAllocator() : pool(PoolSize) {
        for (size_t i = 0; i < PoolSize - 1; i++) {
            pool[i].next = &pool[i + 1];
        }
        pool[PoolSize - 1].next = nullptr;
        freeListHead = &pool[0];
    };
    T* allocate() {
        if (freeListHead == nullptr) return nullptr;
        Slot* currentHead = freeListHead;
        freeListHead = freeListHead->next;
        return reinterpret_cast<T*>(currentHead);
    }
    void deallocate(T* ptr) {
        Slot* slot = reinterpret_cast<Slot*>(ptr);
        slot->next = freeListHead;
        freeListHead = slot;
    }

    template <typename... Args>
    T* construct(Args&&... args) {
        T* ptr = allocate();
        if (ptr == nullptr) return nullptr;
        return new (ptr) T(std::forward<Args>(args)...);
    }

    void destroy(T* ptr) {
        ptr->~T();
        deallocate(ptr);
    }
};