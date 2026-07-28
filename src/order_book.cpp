#include "trading_platform/order_book.h"
#include <iostream>
#include <algorithm>

void OrderBook::addOrder(std::shared_ptr<Order> order) {
    std::lock_guard<std::mutex> lock(mutex_);
    orders_[order->getId()] = order;
    
    if (order->getSide() == OrderSide::BUY) {
        buy_orders_[order->getPrice()].push_back(order);
    } else {
        sell_orders_[order->getPrice()].push_back(order);
    }
    
    matchOrders();
}

bool OrderBook::cancelOrder(int order_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = orders_.find(order_id);
    if (it != orders_.end() && it->second->getStatus() != OrderStatus::FILLED) {
        it->second->cancel();
        return true;
    }
    return false;
}

void OrderBook::matchOrders() {
    bool matched = true;
    while (matched) {
        matched = false;
        
        auto best_bid = buy_orders_.empty() ? buy_orders_.end() : std::prev(buy_orders_.end());
        auto best_ask = sell_orders_.empty() ? sell_orders_.end() : sell_orders_.begin();
        
        if (best_bid != buy_orders_.end() && best_ask != sell_orders_.end() && 
            best_bid->first >= best_ask->first) {
            
            auto& buy_orders = best_bid->second;
            auto& sell_orders = best_ask->second;
            
            auto& buy_order = buy_orders.front();
            auto& sell_order = sell_orders.front();
            
            int trade_qty = std::min(buy_order->getRemainingQuantity(), 
                                    sell_order->getRemainingQuantity());
            double trade_price = best_ask->first;
            
            buy_order->fill(trade_qty);
            sell_order->fill(trade_qty);
            
            trades_.emplace_back(buy_order->getId(), sell_order->getId(), 
                                trade_qty, trade_price);
            
            if (buy_order->getRemainingQuantity() == 0) {
                buy_orders.erase(buy_orders.begin());
            }
            if (sell_order->getRemainingQuantity() == 0) {
                sell_orders.erase(sell_orders.begin());
            }
            
            if (buy_orders.empty()) {
                buy_orders_.erase(best_bid->first);
            }
            if (sell_orders.empty()) {
                sell_orders_.erase(best_ask->first);
            }
            
            matched = true;
        }
    }
}

void OrderBook::printOrderBook() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::cout << "\n=== Order Book ===\n";
    std::cout << "Bids (BUY orders):\n";
    for (auto it = buy_orders_.rbegin(); it != buy_orders_.rend(); ++it) {
        int total_qty = 0;
        for (const auto& order : it->second) {
            if (order->getStatus() != OrderStatus::CANCELLED) {
                total_qty += order->getRemainingQuantity();
            }
        }
        if (total_qty > 0) {
            std::cout << "  $" << it->first << " -> " << total_qty << " shares\n";
        }
    }
    
    std::cout << "Asks (SELL orders):\n";
    for (const auto& [price, orders] : sell_orders_) {
        int total_qty = 0;
        for (const auto& order : orders) {
            if (order->getStatus() != OrderStatus::CANCELLED) {
                total_qty += order->getRemainingQuantity();
            }
        }
        if (total_qty > 0) {
            std::cout << "  $" << price << " -> " << total_qty << " shares\n";
        }
    }
    std::cout << "================\n";
}

void OrderBook::printTradeHistory() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::cout << "\n=== Trade History ===\n";
    for (const auto& trade : trades_) {
        std::cout << "  " << trade.toString() << "\n";
    }
    if (trades_.empty()) {
        std::cout << "  No trades executed\n";
    }
    std::cout << "=====================\n";
}