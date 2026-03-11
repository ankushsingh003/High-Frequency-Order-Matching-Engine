#pragma once
#include <cstdint>

struct Trade {
    uint64_t makerOrderId;
    uint64_t takerOrderId;
    uint32_t price;
    uint32_t quantity;
};
