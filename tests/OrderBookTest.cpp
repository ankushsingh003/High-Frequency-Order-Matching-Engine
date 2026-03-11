#include <gtest/gtest.h>
#include "OrderBook.h"

TEST(OrderBookTest, SimpleMatch) {
    OrderBook book;
    
    auto trades1 = book.addOrder(Order(1, Side::Buy, OrderType::Limit, 100, 10));
    EXPECT_TRUE(trades1.empty());
    
    auto trades2 = book.addOrder(Order(2, Side::Sell, OrderType::Limit, 100, 5));
    ASSERT_EQ(trades2.size(), 1);
    EXPECT_EQ(trades2[0].makerOrderId, 1);
    EXPECT_EQ(trades2[0].takerOrderId, 2);
    EXPECT_EQ(trades2[0].quantity, 5);
    EXPECT_EQ(trades2[0].price, 100);
}

TEST(OrderBookTest, PartialFill) {
    OrderBook book;
    book.addOrder(Order(1, Side::Sell, OrderType::Limit, 105, 10));
    
    auto trades = book.addOrder(Order(2, Side::Buy, OrderType::Limit, 106, 3));
    EXPECT_EQ(trades.size(), 1);
    EXPECT_EQ(trades[0].quantity, 3);
    EXPECT_EQ(trades[0].price, 105);
}
