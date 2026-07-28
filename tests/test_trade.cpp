#include <gtest/gtest.h>
#include "trading_platform/trade.h"
#include <chrono>
#include <thread>

class TradeTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code if needed
    }
};

TEST_F(TradeTest, ConstructorAndGetters) {
    Trade trade(1, 2, 50, 96.00);
    
    EXPECT_EQ(trade.buy_order_id, 1);
    EXPECT_EQ(trade.sell_order_id, 2);
    EXPECT_EQ(trade.quantity, 50);
    EXPECT_EQ(trade.price, 96.00);
    
    // Timestamp should be set automatically
    auto now = std::chrono::system_clock::now();
    auto diff = std::chrono::duration_cast<std::chrono::seconds>(now - trade.timestamp);
    EXPECT_LE(diff.count(), 1);  // Should be within 1 second
}

TEST_F(TradeTest, ToStringFormatting) {
    Trade trade(1, 2, 50, 96.00);
    std::string str = trade.toString();
    
    EXPECT_TRUE(str.find("Trade{ buy:1") != std::string::npos);
    EXPECT_TRUE(str.find("sell:2") != std::string::npos);
    EXPECT_TRUE(str.find("qty:50") != std::string::npos);
    EXPECT_TRUE(str.find("price:96.00") != std::string::npos);
    EXPECT_TRUE(str.find("time:") != std::string::npos);
}

TEST_F(TradeTest, MultipleTradesHaveDifferentTimestamps) {
    Trade trade1(1, 2, 50, 96.00);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    Trade trade2(3, 4, 30, 97.50);
    
    EXPECT_NE(trade1.timestamp, trade2.timestamp);
    EXPECT_GT(trade2.timestamp, trade1.timestamp);
}

TEST_F(TradeTest, TradePricePrecision) {
    Trade trade(1, 2, 10, 95.555);
    std::string str = trade.toString();
    
    // Should round to 2 decimal places
    EXPECT_TRUE(str.find("95.56") != std::string::npos);
}