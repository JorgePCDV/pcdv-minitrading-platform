#ifndef TRADING_PLATFORM_H
#define TRADING_PLATFORM_H

#include "order_book.h"

class TradingPlatform {
public:
    TradingPlatform();
    int placeOrder(OrderSide side, OrderType type, int quantity, double price = 0.0);
    bool cancelOrder(int order_id);
    void printStatus() const;
    
private:
    OrderBook order_book_;
    int next_order_id_;
};

#endif