#include <catch2/catch_test_macros.hpp>

#include "pool_allocator.hpp"

struct TrackedObject {
    int value;
    static inline bool destructorCalled = false;

    TrackedObject(int v) : value(v) { destructorCalled = false; }
    ~TrackedObject() { destructorCalled = true; }
};

TEST_CASE("construct and destroy call ctor/dtor correctly") {
    PoolAllocator<TrackedObject, 8> pool;

    TrackedObject* obj = pool.construct(42);
    REQUIRE(obj != nullptr);
    REQUIRE(obj->value == 42);
    REQUIRE_FALSE(TrackedObject::destructorCalled);

    pool.destroy(obj);
    REQUIRE(TrackedObject::destructorCalled);
}

TEST_CASE("pool exhaustion returns nullptr") {
    PoolAllocator<int, 4> pool;
    REQUIRE(pool.allocate() != nullptr);
    REQUIRE(pool.allocate() != nullptr);
    REQUIRE(pool.allocate() != nullptr);
    REQUIRE(pool.allocate() != nullptr);
    REQUIRE(pool.allocate() == nullptr);
}

TEST_CASE("deallocated slot is reused") {
    PoolAllocator<int, 2> pool;
    int* a = pool.allocate();
    int* b = pool.allocate();
    REQUIRE(pool.allocate() == nullptr);
    pool.deallocate(a);
    int* c = pool.allocate();
    REQUIRE(c == a);
}
