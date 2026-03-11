#include <benchmark/benchmark.h>
#include "OrderBook.h"

static void BM_OrderAdditionEmptyBook(benchmark::State& state) {
    OrderBook book;
    uint64_t orderId = 1;

    for (auto _ : state) {
        Order order(orderId++, Side::Buy, OrderType::Limit, 100, 10);
        book.addOrder(order);
    }
}
BENCHMARK(BM_OrderAdditionEmptyBook);

static void BM_OrderMatching(benchmark::State& state) {
    uint64_t orderId = 1;
    for (auto _ : state) {
        state.PauseTiming();
        OrderBook book;
        // Pre-fill the book with 1000 asks
        for (int i = 0; i < 1000; ++i) {
            book.addOrder(Order(orderId++, Side::Sell, OrderType::Limit, 100 + i, 10));
        }
        state.ResumeTiming();

        // Add a crossing bid that sweeps the first 10 price levels
        Order sweepOrder(orderId++, Side::Buy, OrderType::Limit, 110, 100);
        book.addOrder(sweepOrder);
    }
}
BENCHMARK(BM_OrderMatching);

BENCHMARK_MAIN();
