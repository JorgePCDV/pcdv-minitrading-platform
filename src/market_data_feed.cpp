#include "trading_platform/market_data_feed.h"

MarketDataFeed::MarketDataFeed(const std::string& symbol, double initial_price)
    : symbol_(symbol), gen_(rd_()), price_dist_(0.0, 1.0), volume_dist_(10, 1000) {
    
    current_tick_.symbol = symbol;
    current_tick_.bid_price = initial_price;
    current_tick_.ask_price = initial_price * 1.001;
    current_tick_.bid_volume = volume_dist_(gen_);
    current_tick_.ask_volume = volume_dist_(gen_);
    current_tick_.timestamp = std::chrono::system_clock::now();
}

MarketDataFeed::~MarketDataFeed() {
    stop();
}

void MarketDataFeed::start() {
    if (running_.load()) return;
    
    running_ = true;
    feed_thread_ = std::thread([this]() {
        while (running_.load()) {
            generatePriceTick();
            
            // Notify callback
            if (callback_) {
                PriceTick tick;
                {
                    std::lock_guard<std::mutex> lock(data_mutex_);
                    tick = current_tick_;
                }
                callback_(tick);
            }
            
            std::this_thread::sleep_for(
                std::chrono::milliseconds(update_interval_ms_)
            );
        }
    });
}

void MarketDataFeed::stop() {
    running_ = false;
    if (feed_thread_.joinable()) {
        feed_thread_.join();
    }
}

void MarketDataFeed::setPriceCallback(PriceCallback callback) {
    callback_ = callback;
}

void MarketDataFeed::setUpdateInterval(int milliseconds) {
    update_interval_ms_ = milliseconds;
}

void MarketDataFeed::generatePriceTick() {
    std::lock_guard<std::mutex> lock(data_mutex_);
    
    // Random walk price movement
    double price_change = price_dist_(gen_) * 0.02; // 2% max change
    current_tick_.bid_price = std::max(0.01, current_tick_.bid_price * (1.0 + price_change));
    current_tick_.ask_price = current_tick_.bid_price * (1.001 + price_dist_(gen_) * 0.0005);
    
    current_tick_.bid_volume = volume_dist_(gen_);
    current_tick_.ask_volume = volume_dist_(gen_);
    current_tick_.timestamp = std::chrono::system_clock::now();
}

PriceTick MarketDataFeed::getCurrentTick() const {
    std::lock_guard<std::mutex> lock(data_mutex_);
    return current_tick_;
}