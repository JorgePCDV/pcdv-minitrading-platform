#include <gtest/gtest.h>
#include "trading_platform/order.h"

class OrderTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code if needed
    }
};

TEST_F(OrderTest, ConstructorAndGetters) {
    Order order(1, OrderSide::BUY, OrderType::LIMIT, 100, 95.50);

    EXPECT_EQ(order.getId(), 1);
    EXPECT_EQ(order.getSide(), OrderSide::BUY);
    EXPECT_EQ(order.getType(), OrderType::LIMIT);
    EXPECT_EQ(order.getRemainingQuantity(), 100);
    EXPECT_EQ(order.getPrice(), 95.50);
    EXPECT_EQ(order.getStatus(), OrderStatus::PENDING);
}

TEST_F(OrderTest, MarketOrderWithoutPrice) {
    Order order(2, OrderSide::SELL, OrderType::MARKET, 50);

    EXPECT_EQ(order.getId(), 2);
    EXPECT_EQ(order.getSide(), OrderSide::SELL);
    EXPECT_EQ(order.getType(), OrderType::MARKET);
    EXPECT_EQ(order.getRemainingQuantity(), 50);
    EXPECT_EQ(order.getPrice(), 0.0);  // Market orders have no price
    EXPECT_EQ(order.getStatus(), OrderStatus::PENDING);
}

TEST_F(OrderTest, FullFillOrder) {
    Order order(3, OrderSide::BUY, OrderType::LIMIT, 100, 95.50);

    order.fill(100);
    EXPECT_EQ(order.getRemainingQuantity(), 0);
    EXPECT_EQ(order.getStatus(), OrderStatus::FILLED);
}

TEST_F(OrderTest, PartialFillOrder) {
    Order order(4, OrderSide::BUY, OrderType::LIMIT, 100, 95.50);

    order.fill(60);
    EXPECT_EQ(order.getRemainingQuantity(), 40);
    EXPECT_EQ(order.getStatus(), OrderStatus::PARTIALLY_FILLED);
}

TEST_F(OrderTest, OverfillOrder) {
    Order order(5, OrderSide::BUY, OrderType::LIMIT, 100, 95.50);

    order.fill(150);  // More than available
    EXPECT_EQ(order.getRemainingQuantity(), 100);  // Should not change
    EXPECT_EQ(order.getStatus(), OrderStatus::PENDING);
}

TEST_F(OrderTest, CancelPendingOrder) {
    Order order(6, OrderSide::BUY, OrderType::LIMIT, 100, 95.50);

    order.cancel();
    EXPECT_EQ(order.getStatus(), OrderStatus::CANCELLED);
}

TEST_F(OrderTest, CancelPartiallyFilledOrder) {
    Order order(7, OrderSide::BUY, OrderType::LIMIT, 100, 95.50);

    order.fill(40);
    EXPECT_EQ(order.getStatus(), OrderStatus::PARTIALLY_FILLED);

    order.cancel();
    EXPECT_EQ(order.getStatus(), OrderStatus::CANCELLED);
}

TEST_F(OrderTest, CannotCancelFilledOrder) {
    Order order(8, OrderSide::BUY, OrderType::LIMIT, 100, 95.50);

    order.fill(100);
    EXPECT_EQ(order.getStatus(), OrderStatus::FILLED);

    order.cancel();
    EXPECT_EQ(order.getStatus(), OrderStatus::FILLED);  // Should remain FILLED
}

TEST_F(OrderTest, CannotCancelAlreadyCancelledOrder) {
    Order order(9, OrderSide::BUY, OrderType::LIMIT, 100, 95.50);

    order.cancel();
    EXPECT_EQ(order.getStatus(), OrderStatus::CANCELLED);

    order.cancel();  // Second cancel should have no effect
    EXPECT_EQ(order.getStatus(), OrderStatus::CANCELLED);
}

TEST_F(OrderTest, ToStringFormatting) {
    Order order(10, OrderSide::BUY, OrderType::LIMIT, 100, 95.50);
    std::string str = order.toString();

    EXPECT_TRUE(str.find("Order{id:10") != std::string::npos);
    EXPECT_TRUE(str.find("BUY") != std::string::npos);
    EXPECT_TRUE(str.find("LIMIT") != std::string::npos);
    EXPECT_TRUE(str.find("100") != std::string::npos);
    EXPECT_TRUE(str.find("95.5") != std::string::npos);
}