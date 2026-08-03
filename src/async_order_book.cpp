#include "trading_platform/async_order_book.h"
#include <chrono>
#include <iostream>

AsyncOrderBook::AsyncOrderBook(size_t num_threads)
    : thread_pool_(num_threads), next_order_id_(1) {}

AsyncOrderBook::~AsyncOrderBook() {
    thread_pool_.waitForCompletion();
}

int AsyncOrderBook::getNextOrderId() {
    return next_order_id_++;
}

std::future<int> AsyncOrderBook::addOrderAsync(std::shared_ptr<Order> order,
                                               OrderPriority priority) {
    // Add to pending queue for statistics
    {
        std::lock_guard<std::mutex> lock(pending_mutex_);
        pending_orders_.push(order);
    }

    // Enqueue the order processing task
    return thread_pool_.enqueue([this, order]() -> int {
        processOrder(order);
        return order->getId();
    });
}

std::future<bool> AsyncOrderBook::cancelOrderAsync(int order_id) {
    return thread_pool_.enqueue([this, order_id]() -> bool {
        return processCancel(order_id);
    });
}

void AsyncOrderBook::processOrder(std::shared_ptr<Order> order) {
    // Simulate some processing time (e.g., validation, risk checks)
    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    order_book_.addOrder(order);

    // Update statistics
    processed_count_++;
    {
        std::lock_guard<std::mutex> lock(pending_mutex_);
        if (!pending_orders_.empty()) {
            pending_orders_.pop();
        }
    }
}

bool AsyncOrderBook::processCancel(int order_id) {
    return order_book_.cancelOrder(order_id);
}

void AsyncOrderBook::printOrderBook() const {
    std::cout << "\n[ASYNC] Current Order Book State:\n";
    order_book_.printOrderBook();
}

void AsyncOrderBook::printTradeHistory() const {
    std::cout << "\n[ASYNC] Trade History:\n";
    order_book_.printTradeHistory();
}

size_t AsyncOrderBook::getPendingOrderCount() const {
    std::lock_guard<std::mutex> lock(pending_mutex_);
    return pending_orders_.size();
}

size_t AsyncOrderBook::getProcessedOrderCount() const {
    return processed_count_.load();
}