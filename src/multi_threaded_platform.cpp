#include "trading_platform/multi_threaded_platform.h"
#include <iostream>
#include <iomanip>
#include <thread>

MultiThreadedPlatform::MultiThreadedPlatform()
    : order_book_(4),  // 4 threads for order processing
      market_feed_("AAPL", 150.0) {

    // Set up market data callback
    market_feed_.setPriceCallback(
        [this](const PriceTick& tick) {
            onMarketTick(tick);
        }
    );
}

MultiThreadedPlatform::~MultiThreadedPlatform() {
    stop();
}

void MultiThreadedPlatform::start() {
    running_ = true;
    market_feed_.start();
    risk_manager_.start();

    // Start stats reporting thread
    stats_thread_ = std::thread([this]() { statsLoop(); });

    std::cout << "[PLATFORM] Multi-threaded platform started\n";
    std::cout << "[PLATFORM] Order processing threads: 4\n";
    std::cout << "[PLATFORM] Market data feed: AAPL @ $150.00\n";
}

void MultiThreadedPlatform::stop() {
    running_ = false;
    market_feed_.stop();
    risk_manager_.stop();

    if (stats_thread_.joinable()) {
        stats_thread_.join();
    }

    std::cout << "[PLATFORM] Platform stopped\n";
}

int MultiThreadedPlatform::placeOrderAsync(OrderSide side, OrderType type,
                                           int quantity, double price) {
    // Get next order ID from async order book
    int order_id = order_book_.getNextOrderId();
    auto order = std::make_shared<Order>(order_id, side, type, quantity, price);

    // Risk check (synchronous for critical orders)
    if (!risk_manager_.validateOrder(order)) {
        std::cerr << "[RISK] Order rejected by risk manager\n";
        return -1;
    }

    // Submit asynchronously
    auto future = order_book_.addOrderAsync(order);

    // Optional: Could store future for later checking
    std::cout << "[PLATFORM] Order " << order_id << " submitted asynchronously\n";
    return order_id;
}

bool MultiThreadedPlatform::cancelOrderAsync(int order_id) {
    auto future = order_book_.cancelOrderAsync(order_id);
    // Could wait for result or just fire and forget
    std::cout << "[PLATFORM] Cancellation request for order " << order_id << " submitted\n";
    return true; // Return immediately, actual result via future
}

void MultiThreadedPlatform::onMarketTick(const PriceTick& tick) {
    // Update risk manager with current market data
    // This runs in the market data thread
    static int tick_count = 0;
    tick_count++;

    // Only print every 10 ticks to avoid spam
    if (tick_count % 10 == 0) {
        std::cout << "[MARKET] " << tick.symbol
                  << " Bid: $" << std::fixed << std::setprecision(2) << tick.bid_price
                  << " Ask: $" << tick.ask_price
                  << " Vol: " << tick.bid_volume << "/" << tick.ask_volume << "\n";
    }
    
    // Optionally auto-trade based on market conditions
    processAutoOrders(tick);
}

void MultiThreadedPlatform::processAutoOrders(const PriceTick& tick) {
    // Example: Simple moving average crossover strategy
    static double last_price = tick.bid_price;
    static int count = 0;
    
    count++;
    if (count % 50 == 0) { // Every 50 ticks
        double price_change = (tick.bid_price - last_price) / last_price;
        if (price_change > 0.01) { // Price went up 1% -> buy
            std::cout << "[AUTO] Triggering buy order\n";
            placeOrderAsync(OrderSide::BUY, OrderType::LIMIT, 100, tick.bid_price);
        } else if (price_change < -0.01) { // Price went down 1% -> sell
            std::cout << "[AUTO] Triggering sell order\n";
            placeOrderAsync(OrderSide::SELL, OrderType::LIMIT, 100, tick.ask_price);
        }
        last_price = tick.bid_price;
    }
}

void MultiThreadedPlatform::statsLoop() {
    while (running_.load()) {
        std::this_thread::sleep_for(std::chrono::seconds(5));
        printStats();
    }
}

void MultiThreadedPlatform::printStats() const {
    std::cout << "\n[STATS] ================================\n";
    std::cout << "  Pending Orders: " << order_book_.getPendingOrderCount() << "\n";
    std::cout << "  Processed: " << order_book_.getProcessedOrderCount() << "\n";
    std::cout << "  Total PnL: $" << std::fixed << std::setprecision(2) 
              << risk_manager_.getTotalPnL() << "\n";
    std::cout << "  Exposure: $" << risk_manager_.getCurrentExposure() << "\n";
    std::cout << "  Active Orders: " << risk_manager_.getActiveOrderCount() << "\n";
    std::cout << "  Market Price: $" << market_feed_.getCurrentTick().bid_price << "\n";
    std::cout << "========================================\n";
}

void MultiThreadedPlatform::printStatus() const {
    order_book_.printOrderBook();
    order_book_.printTradeHistory();
}