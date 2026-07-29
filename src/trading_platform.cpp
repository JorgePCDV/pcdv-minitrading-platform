#include "trading_platform/trading_platform.h"
#include <iostream>
#include <memory>

TradingPlatform::TradingPlatform() : next_order_id_(1) {}

int TradingPlatform::placeOrder(OrderSide side, OrderType type, int quantity, double price) {
    int order_id = next_order_id_++;
    auto order = std::make_shared<Order>(order_id, side, type, quantity, price);
    
    if (type == OrderType::LIMIT && price <= 0) {
        std::cerr << "Error: Limit orders require a positive price\n";
        return -1;
    }
    
    std::cout << "Placing: " << order->toString() << "\n";
    order_book_.addOrder(order);
    return order_id;
}

bool TradingPlatform::cancelOrder(int order_id) {
    std::cout << "Cancelling order " << order_id << "\n";
    return order_book_.cancelOrder(order_id);
}

void TradingPlatform::printStatus() const {
    order_book_.printOrderBook();
    order_book_.printTradeHistory();
}

void TradingPlatform::printOrderBook() const {
    order_book_.printOrderBook();
}

void TradingPlatform::printTradeHistory() const {
    order_book_.printTradeHistory();
}