#include "trading_platform/thread_pool.h"

ThreadPool::ThreadPool(size_t num_threads) : stop_(false), active_tasks_(0) {
    if (num_threads == 0) {
        num_threads = std::thread::hardware_concurrency();
    }
    
    for (size_t i = 0; i < num_threads; ++i) {
        workers_.emplace_back([this] {
            while (true) {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(queue_mutex_);
                    condition_.wait(lock, [this] {
                        return stop_ || !tasks_.empty();
                    });
                    
                    if (stop_ && tasks_.empty()) {
                        return;
                    }
                    
                    task = std::move(tasks_.front());
                    tasks_.pop();
                }
                task();
                active_tasks_--;
            }
        });
    }
}

ThreadPool::~ThreadPool() {
    stop_ = true;
    condition_.notify_all();
    for (std::thread& worker : workers_) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

void ThreadPool::waitForCompletion() {
    while (active_tasks_ > 0 || !tasks_.empty()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

size_t ThreadPool::getPendingTasks() const {
    std::unique_lock<std::mutex> lock(queue_mutex_);
    return tasks_.size();
}