#ifndef MARKET_DATA_FEED_H
#define MARKET_DATA_FEED_H

#include <atomic>
#include <thread>
#include <functional>
#include <random>
#include <chrono>
#include <map>
#include <mutex>

struct PriceTick {
    std::string symbol;
    double bid_price;
    double ask_price;
    int bid_volume;
    int ask_volume;
    std::chrono::system_clock::time_point timestamp;
};

class MarketDataFeed {
public:
    using PriceCallback = std::function<void(const PriceTick&)>;
    
    MarketDataFeed(const std::string& symbol, double initial_price = 100.0);
    ~MarketDataFeed();
    
    void start();
    void stop();
    void setPriceCallback(PriceCallback callback);
    void setUpdateInterval(int milliseconds);
    
    // Get current market data
    PriceTick getCurrentTick() const;
    
private:
    std::string symbol_;
    std::atomic<bool> running_{false};
    std::thread feed_thread_;
    PriceCallback callback_;
    mutable std::mutex data_mutex_;
    PriceTick current_tick_;
    int update_interval_ms_{100};
    
    std::random_device rd_;
    std::mt19937 gen_;
    std::normal_distribution<> price_dist_;
    std::uniform_int_distribution<> volume_dist_;
    
    void generatePriceTick();
    PriceTick generateRandomTick();
};

#endif