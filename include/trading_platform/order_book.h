#ifndef ORDER_BOOK_H
#define ORDER_BOOK_H

#include <map>
#include <vector>
#include <memory>
#include <mutex>
#include "order.h"
#include "trade.h"

class OrderBook {
public:
    void addOrder(std::shared_ptr<Order> order);
    bool cancelOrder(int order_id);
    void printOrderBook() const;
    void printTradeHistory() const;
    
private:
    std::map<int, std::shared_ptr<Order>> orders_;
    std::map<double, std::vector<std::shared_ptr<Order>>> buy_orders_;
    std::map<double, std::vector<std::shared_ptr<Order>>> sell_orders_;
    std::vector<Trade> trades_;
    mutable std::mutex mutex_;
    
    void matchOrders();
};

#endif