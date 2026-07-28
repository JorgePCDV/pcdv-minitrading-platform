#include "trading_platform/trading_platform.h"
#include <iostream>

int main() {
    TradingPlatform platform;

    std::cout << "=== Mini Trading Platform Demo ===\n\n";

    // Place some limit orders
    int order1 = platform.placeOrder(OrderSide::BUY, OrderType::LIMIT, 100, 95.50);
    int order2 = platform.placeOrder(OrderSide::SELL, OrderType::LIMIT, 50, 96.00);
    int order3 = platform.placeOrder(OrderSide::BUY, OrderType::LIMIT, 200, 96.50);
    int order4 = platform.placeOrder(OrderSide::SELL, OrderType::LIMIT, 150, 95.75);
    int order5 = platform.placeOrder(OrderSide::BUY, OrderType::MARKET, 80);

    platform.printStatus();

    // Add more orders to trigger matching
    std::cout << "\n--- Adding matching orders ---\n";
    int order6 = platform.placeOrder(OrderSide::SELL, OrderType::LIMIT, 100, 95.00);
    int order7 = platform.placeOrder(OrderSide::BUY, OrderType::LIMIT, 120, 97.00);

    platform.printStatus();

    // Cancel an order
    platform.cancelOrder(order2);
    platform.printStatus();

    return 0;
}