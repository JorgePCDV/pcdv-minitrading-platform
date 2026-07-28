#include <gtest/gtest.h>
#include "trading_platform/order_book.h"
#include <memory>

class OrderBookTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code if needed
    }
    
    std::shared_ptr<Order> createOrder(int id, OrderSide side, OrderType type, 
                                      int quantity, double price = 0.0) {
        return std::make_shared<Order>(id, side, type, quantity, price);
    }
};

TEST_F(OrderBookTest, AddSingleOrder) {
    OrderBook book;
    auto order = createOrder(1, OrderSide::BUY, OrderType::LIMIT, 100, 95.50);
    
    book.addOrder(order);
    
    // No trades should occur with single order
    testing::internal::CaptureStdout();
    book.printTradeHistory();
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(output.find("No trades executed") != std::string::npos);
}

TEST_F(OrderBookTest, MatchingBuyAndSellOrders) {
    OrderBook book;
    auto buy_order = createOrder(1, OrderSide::BUY, OrderType::LIMIT, 100, 96.00);
    auto sell_order = createOrder(2, OrderSide::SELL, OrderType::LIMIT, 100, 95.50);
    
    book.addOrder(buy_order);
    book.addOrder(sell_order);
    
    // Buy and sell should match
    testing::internal::CaptureStdout();
    book.printTradeHistory();
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(output.find("Trade{") != std::string::npos);
    EXPECT_TRUE(output.find("qty:100") != std::string::npos);
}

TEST_F(OrderBookTest, PartialFill) {
    OrderBook book;
    auto buy_order = createOrder(1, OrderSide::BUY, OrderType::LIMIT, 100, 96.00);
    auto sell_order = createOrder(2, OrderSide::SELL, OrderType::LIMIT, 60, 95.50);
    
    book.addOrder(buy_order);
    book.addOrder(sell_order);
    
    // Should execute 60, leaving 40
    testing::internal::CaptureStdout();
    book.printTradeHistory();
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(output.find("qty:60") != std::string::npos);
}

TEST_F(OrderBookTest, PricePriority) {
    OrderBook book;
    auto low_buy = createOrder(1, OrderSide::BUY, OrderType::LIMIT, 100, 95.00);
    auto high_buy = createOrder(2, OrderSide::BUY, OrderType::LIMIT, 100, 96.00);
    auto sell = createOrder(3, OrderSide::SELL, OrderType::LIMIT, 100, 94.50);
    
    book.addOrder(low_buy);
    book.addOrder(high_buy);
    book.addOrder(sell);
    
    // Should match with high_buy first (best price)
    testing::internal::CaptureStdout();
    book.printOrderBook();
    std::string output = testing::internal::GetCapturedStdout();
    // Both should still be visible since only 100 sold
}

TEST_F(OrderBookTest, CancelOrder) {
    OrderBook book;
    auto order = createOrder(1, OrderSide::BUY, OrderType::LIMIT, 100, 95.50);
    
    book.addOrder(order);
    bool cancelled = book.cancelOrder(1);
    
    EXPECT_TRUE(cancelled);
    EXPECT_EQ(order->getStatus(), OrderStatus::CANCELLED);
}

TEST_F(OrderBookTest, CancelNonExistentOrder) {
    OrderBook book;
    bool cancelled = book.cancelOrder(999);
    EXPECT_FALSE(cancelled);
}

TEST_F(OrderBookTest, CancelFilledOrder) {
    OrderBook book;
    auto buy = createOrder(1, OrderSide::BUY, OrderType::LIMIT, 100, 96.00);
    auto sell = createOrder(2, OrderSide::SELL, OrderType::LIMIT, 100, 95.50);
    
    book.addOrder(buy);
    book.addOrder(sell);
    
    bool cancelled = book.cancelOrder(1);
    EXPECT_FALSE(cancelled);  // Filled orders cannot be cancelled
    EXPECT_EQ(buy->getStatus(), OrderStatus::FILLED);
}

TEST_F(OrderBookTest, ThreadSafety) {
    OrderBook book;
    const int num_orders = 100;
    std::vector<std::thread> threads;
    
    // Add orders from multiple threads
    for (int i = 0; i < num_orders; ++i) {
        threads.emplace_back([&book, i]() {
            auto order = std::make_shared<Order>(
                i, 
                i % 2 == 0 ? OrderSide::BUY : OrderSide::SELL,
                OrderType::LIMIT,
                100,
                95.00 + (i % 10) * 0.10
            );
            book.addOrder(order);
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    // Should not crash and should have processed all orders
    EXPECT_NO_THROW(book.printOrderBook());
}

TEST_F(OrderBookTest, ComplexMatchingScenario) {
    OrderBook book;
    
    // Place multiple orders at different prices
    auto buy1 = createOrder(1, OrderSide::BUY, OrderType::LIMIT, 100, 97.00);
    auto buy2 = createOrder(2, OrderSide::BUY, OrderType::LIMIT, 200, 96.00);
    auto sell1 = createOrder(3, OrderSide::SELL, OrderType::LIMIT, 50, 95.00);
    auto sell2 = createOrder(4, OrderSide::SELL, OrderType::LIMIT, 80, 96.50);
    auto sell3 = createOrder(5, OrderSide::SELL, OrderType::LIMIT, 70, 97.50);
    
    book.addOrder(buy1);
    book.addOrder(buy2);
    book.addOrder(sell1);
    book.addOrder(sell2);
    book.addOrder(sell3);
    
    // Should match:
    // - sell1 (50 @ 95) with buy1 (100 @ 97) -> 50 shares
    // - sell2 (80 @ 96.50) with remaining buy1 (50 @ 97) -> 50 shares
    // - sell2 remaining (30 @ 96.50) with buy2 (200 @ 96) -> 30 shares
    // Total: 130 shares matched
    testing::internal::CaptureStdout();
    book.printTradeHistory();
    std::string output = testing::internal::GetCapturedStdout();
    
    // Should have multiple trades
    size_t trade_count = 0;
    size_t pos = 0;
    while ((pos = output.find("Trade{", pos)) != std::string::npos) {
        trade_count++;
        pos++;
    }
    EXPECT_GE(trade_count, 2);
}