#ifndef TRADE_H
#define TRADE_H

#include <chrono>
#include <string>
#include <sstream>
#include <iomanip>

struct Trade {
    int buy_order_id;
    int sell_order_id;
    int quantity;
    double price;
    std::chrono::system_clock::time_point timestamp;
    
    Trade(int buyId, int sellId, int qty, double p);
    std::string toString() const;
};

#endif