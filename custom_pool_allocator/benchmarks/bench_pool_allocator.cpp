#include <benchmark/benchmark.h>

#include "pool_allocator.hpp"

struct Order {
    int id;
    double price;
    int quantity;
    Order(int id_, double price_, int qty_) : id(id_), price(price_), quantity(qty_) {}
};

// --- Pool allocator: construct + destroy ---
static void BM_PoolAllocator(benchmark::State& state) {
    PoolAllocator<Order, 1024> pool;
    for (auto _ : state) {
        Order* o = pool.construct(1, 100.5, 10);
        benchmark::DoNotOptimize(o);
        pool.destroy(o);
    }
}
BENCHMARK(BM_PoolAllocator);

// --- Baseline: new + delete ---
static void BM_NewDelete(benchmark::State& state) {
    for (auto _ : state) {
        Order* o = new Order(1, 100.5, 10);
        benchmark::DoNotOptimize(o);
        delete o;
    }
}
BENCHMARK(BM_NewDelete);

BENCHMARK_MAIN();