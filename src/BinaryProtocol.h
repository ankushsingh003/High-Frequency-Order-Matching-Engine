#pragma once
#include <cstdint>
#include "Order.h"

#pragma pack(push, 1)
struct BinaryOrderMessage {
    uint8_t  messageType; // 'O' for Order
    uint8_t  side;        // 0 = Buy, 1 = Sell
    uint8_t  orderType;   // 0=Market, 1=Limit, 2=IOC, 3=FOK
    uint32_t price;
    uint32_t quantity;
};
#pragma pack(pop)

class BinaryProtocolParser {
public:
    static bool parse(const uint8_t* buffer, size_t length, Order& outOrder, uint64_t nextOrderId) {
        if (length < sizeof(BinaryOrderMessage)) return false;
        
        // Zero-copy deserialization using reinterpret_cast
        const BinaryOrderMessage* msg = reinterpret_cast<const BinaryOrderMessage*>(buffer);
        
        if (msg->messageType != 'O') return false;

        outOrder.orderId = nextOrderId;
        outOrder.side = static_cast<Side>(msg->side);
        outOrder.type = static_cast<OrderType>(msg->orderType);
        outOrder.price = msg->price;
        outOrder.quantity = msg->quantity;
        
        return true;
    }
};
