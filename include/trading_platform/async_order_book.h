#ifndef ASYNC_ORDER_BOOK_H
#define ASYNC_ORDER_BOOK_H

#include "order_book.h"
#include "thread_pool.h"
#include <memory>
#include <future>
#include <queue>
#include <atomic>
#include <mutex>

enum class OrderPriority { HIGH, NORMAL, LOW };

struct OrderRequest {
    std::shared_ptr<Order> order;
    OrderPriority priority;
    std::chrono::system_clock::time_point timestamp;

    OrderRequest(std::shared_ptr<Order> o, OrderPriority p = OrderPriority::NORMAL)
        : order(o), priority(p), timestamp(std::chrono::system_clock::now()) {}
};

class AsyncOrderBook {
public:
    AsyncOrderBook(size_t num_threads = 4);
    ~AsyncOrderBook();

    // Async order placement - returns future for order ID
    std::future<int> addOrderAsync(std::shared_ptr<Order> order,
                                   OrderPriority priority = OrderPriority::NORMAL);

    // Async cancellation
    std::future<bool> cancelOrderAsync(int order_id);

    // Get next order ID (thread-safe)
    int getNextOrderId();

    // Sync methods (for status queries)
    void printOrderBook() const;
    void printTradeHistory() const;

    // Get statistics
    size_t getPendingOrderCount() const;
    size_t getProcessedOrderCount() const;

private:
    OrderBook order_book_;
    ThreadPool thread_pool_;
    mutable std::mutex pending_mutex_;
    std::queue<std::shared_ptr<Order>> pending_orders_;
    std::atomic<size_t> processed_count_{0};
    std::atomic<int> next_order_id_{1};  // Added atomic counter
    
    void processOrder(std::shared_ptr<Order> order);
    bool processCancel(int order_id);
};

#endif