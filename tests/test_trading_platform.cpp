#include <gtest/gtest.h>
#include "trading_platform/trading_platform.h"

class TradingPlatformTest : public ::testing::Test {
protected:
    void SetUp() override {
        platform = std::make_unique<TradingPlatform>();
    }
    
    std::unique_ptr<TradingPlatform> platform;
};

TEST_F(TradingPlatformTest, PlaceLimitOrder) {
    int order_id = platform->placeOrder(OrderSide::BUY, OrderType::LIMIT, 100, 95.50);
    
    EXPECT_GT(order_id, 0);
    EXPECT_NE(order_id, -1);
}

TEST_F(TradingPlatformTest, PlaceMarketOrder) {
    int order_id = platform->placeOrder(OrderSide::BUY, OrderType::MARKET, 100);
    
    EXPECT_GT(order_id, 0);
    EXPECT_NE(order_id, -1);
}

TEST_F(TradingPlatformTest, InvalidLimitOrder) {
    int order_id = platform->placeOrder(OrderSide::BUY, OrderType::LIMIT, 100, -5.00);
    
    EXPECT_EQ(order_id, -1);  // Should return error
}

TEST_F(TradingPlatformTest, PlaceMultipleOrders) {
    int id1 = platform->placeOrder(OrderSide::BUY, OrderType::LIMIT, 100, 95.50);
    int id2 = platform->placeOrder(OrderSide::SELL, OrderType::LIMIT, 50, 96.00);
    int id3 = platform->placeOrder(OrderSide::BUY, OrderType::LIMIT, 200, 96.50);
    
    EXPECT_NE(id1, id2);
    EXPECT_NE(id2, id3);
    EXPECT_NE(id1, id3);
    EXPECT_GT(id1, 0);
    EXPECT_GT(id2, 0);
    EXPECT_GT(id3, 0);
}

TEST_F(TradingPlatformTest, CancelOrderSuccess) {
    int order_id = platform->placeOrder(OrderSide::BUY, OrderType::LIMIT, 100, 95.50);
    
    bool cancelled = platform->cancelOrder(order_id);
    EXPECT_TRUE(cancelled);
}

TEST_F(TradingPlatformTest, CancelOrderFailure) {
    bool cancelled = platform->cancelOrder(999);
    EXPECT_FALSE(cancelled);
}

TEST_F(TradingPlatformTest, MatchingOrders) {
    int buy_id = platform->placeOrder(OrderSide::BUY, OrderType::LIMIT, 100, 97.00);
    int sell_id = platform->placeOrder(OrderSide::SELL, OrderType::LIMIT, 100, 96.00);
    
    EXPECT_GT(buy_id, 0);
    EXPECT_GT(sell_id, 0);
    
    testing::internal::CaptureStdout();
    platform->printStatus();
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_FALSE(output.empty());
}

TEST_F(TradingPlatformTest, MarketOrderExecution) {
    platform->placeOrder(OrderSide::SELL, OrderType::LIMIT, 100, 95.00);
    
    int buy_id = platform->placeOrder(OrderSide::BUY, OrderType::MARKET, 80);
    
    EXPECT_GT(buy_id, 0);
    EXPECT_NE(buy_id, -1);
}

TEST_F(TradingPlatformTest, PrintStatus) {
    platform->placeOrder(OrderSide::BUY, OrderType::LIMIT, 100, 95.50);
    platform->placeOrder(OrderSide::SELL, OrderType::LIMIT, 50, 96.00);
    platform->placeOrder(OrderSide::BUY, OrderType::MARKET, 80);
    
    testing::internal::CaptureStdout();
    platform->printStatus();
    std::string output = testing::internal::GetCapturedStdout();
    
    EXPECT_TRUE(output.find("Order Book") != std::string::npos);
    EXPECT_TRUE(output.find("Trade History") != std::string::npos);
}

TEST_F(TradingPlatformTest, OrderIdIncrements) {
    int id1 = platform->placeOrder(OrderSide::BUY, OrderType::LIMIT, 100, 95.50);
    int id2 = platform->placeOrder(OrderSide::BUY, OrderType::LIMIT, 100, 95.50);
    int id3 = platform->placeOrder(OrderSide::BUY, OrderType::LIMIT, 100, 95.50);
    
    EXPECT_EQ(id1 + 1, id2);
    EXPECT_EQ(id2 + 1, id3);
}

TEST_F(TradingPlatformTest, CancelPendingOrder) {
    int order_id = platform->placeOrder(OrderSide::BUY, OrderType::LIMIT, 100, 95.50);
    EXPECT_NE(order_id, -1);

    bool cancelled = platform->cancelOrder(order_id);
    EXPECT_TRUE(cancelled);
}

TEST_F(TradingPlatformTest, ComplexOrderSequence) {
    int order1 = platform->placeOrder(OrderSide::BUY, OrderType::LIMIT, 100, 95.00);
    int order2 = platform->placeOrder(OrderSide::SELL, OrderType::LIMIT, 50, 96.00);
    int order3 = platform->placeOrder(OrderSide::BUY, OrderType::LIMIT, 200, 94.00);
    int order4 = platform->placeOrder(OrderSide::SELL, OrderType::LIMIT, 150, 97.00);

    EXPECT_NE(order1, -1);
    EXPECT_NE(order2, -1);
    EXPECT_NE(order3, -1);
    EXPECT_NE(order4, -1);

    bool cancelled = platform->cancelOrder(order2);
    EXPECT_TRUE(cancelled);

    int order5 = platform->placeOrder(OrderSide::BUY, OrderType::MARKET, 80);
    EXPECT_NE(order5, -1);

    testing::internal::CaptureStdout();
    platform->printStatus();
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_FALSE(output.empty());
}