#pragma once
#include <cstdint>

enum class Side : uint8_t {
    Buy,
    Sell
};

enum class OrderType : uint8_t {
    Market,
    Limit,
    IOC,    // Immediate-Or-Cancel
    FOK     // Fill-Or-Kill
};

struct Order {
    uint64_t orderId;
    uint32_t price;
    uint32_t quantity;
    Side side;
    OrderType type;

    // Intrusive pointers for zero-allocation doubly linked list
    Order* prev = nullptr;
    Order* next = nullptr;

    Order() = default;
    Order(uint64_t id, Side side, OrderType type, uint32_t price, uint32_t quantity)
        : orderId(id), price(price), quantity(quantity), side(side), type(type) {}
};

