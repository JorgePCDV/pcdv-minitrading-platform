#include "trading_platform/risk_manager.h"
#include <iostream>

RiskManager::RiskManager() 
    : last_reset_time_(std::chrono::steady_clock::now()) {}

RiskManager::~RiskManager() {
    stop();
}

void RiskManager::start() {
    if (running_.load()) return;
    running_ = true;
    monitor_thread_ = std::thread([this]() { monitorLoop(); });
}

void RiskManager::stop() {
    running_ = false;
    if (monitor_thread_.joinable()) {
        monitor_thread_.join();
    }
}

void RiskManager::monitorLoop() {
    while (running_.load()) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        checkRiskLimits();
        resetCounters();
    }
}

bool RiskManager::validateOrder(const std::shared_ptr<Order>& order) const {
    // Check order size
    if (order->getRemainingQuantity() > limits_.max_order_size) {
        std::cerr << "[RISK] Order size exceeds limit: " 
                  << order->getRemainingQuantity() << "\n";
        return false;
    }
    
    // Check order rate
    if (order_count_this_second_.load() > limits_.max_order_count_per_second) {
        std::cerr << "[RISK] Order rate limit exceeded\n";
        return false;
    }
    
    // Check total exposure
    double order_value = order->getRemainingQuantity() * 
                        (order->getPrice() > 0 ? order->getPrice() : 100.0);
    if (current_exposure_.load() + order_value > limits_.max_position_size) {
        std::cerr << "[RISK] Position size limit would be exceeded\n";
        return false;
    }
    
    return true;
}

void RiskManager::updatePosition(int order_id, const Position& position) {
    std::lock_guard<std::mutex> lock(positions_mutex_);
    positions_[order_id] = position;
    total_pnl_ += position.current_pnl;
    current_exposure_ += position.entry_price * position.quantity;
    order_count_this_second_++;
}

bool RiskManager::isWithinLimits() const {
    return total_pnl_.load() > limits_.max_daily_pnl_loss &&
           current_exposure_.load() < limits_.max_position_size;
}

void RiskManager::checkRiskLimits() {
    if (!isWithinLimits()) {
        std::cerr << "[RISK] WARNING: Risk limits breached!\n";
        std::cerr << "  Total PnL: $" << total_pnl_.load() << "\n";
        std::cerr << "  Exposure: $" << current_exposure_.load() << "\n";
        // Could trigger emergency actions here
    }
}

void RiskManager::resetCounters() {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
        now - last_reset_time_
    ).count();
    
    if (elapsed >= 1) {
        order_count_this_second_ = 0;
        last_reset_time_ = now;
    }
}

double RiskManager::getTotalPnL() const {
    return total_pnl_.load();
}

double RiskManager::getCurrentExposure() const {
    return current_exposure_.load();
}

int RiskManager::getActiveOrderCount() const {
    std::lock_guard<std::mutex> lock(positions_mutex_);
    return positions_.size();
}