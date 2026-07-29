#ifndef MULTI_THREADED_PLATFORM_H
#define MULTI_THREADED_PLATFORM_H

#include "async_order_book.h"
#include "market_data_feed.h"
#include "risk_manager.h"
#include <atomic>
#include <thread>
#include <memory>

class MultiThreadedPlatform {
public:
    MultiThreadedPlatform();
    ~MultiThreadedPlatform();
    
    void start();
    void stop();
    
    // Trading operations (non-blocking)
    int placeOrderAsync(OrderSide side, OrderType type, int quantity, double price = 0.0);
    bool cancelOrderAsync(int order_id);
    
    // Status queries (thread-safe)
    void printStatus() const;
    void printStats() const;
    
private:
    AsyncOrderBook order_book_;
    MarketDataFeed market_feed_;
    RiskManager risk_manager_;
    
    std::atomic<bool> running_{false};
    std::thread stats_thread_;
    
    void statsLoop();
    void onMarketTick(const PriceTick& tick);
    void processAutoOrders(const PriceTick& tick);
};

#endif