#ifndef RISK_MANAGER_H
#define RISK_MANAGER_H

#include <atomic>
#include <thread>
#include <map>
#include <mutex>
#include <chrono>

#include "order.h"

struct Position {
    int order_id;
    double entry_price;
    int quantity;
    double current_pnl;
    std::chrono::system_clock::time_point entry_time;
};

struct RiskLimits {
    double max_position_size = 1000000.0;
    double max_daily_pnl_loss = -50000.0;
    double max_order_size = 10000.0;
    int max_order_count_per_second = 100;
};

class RiskManager {
public:
    RiskManager();
    ~RiskManager();
    
    void start();
    void stop();
    
    // Risk checks
    bool validateOrder(const std::shared_ptr<Order>& order) const;
    void updatePosition(int order_id, const Position& position);
    bool isWithinLimits() const;
    
    // Monitoring
    double getTotalPnL() const;
    double getCurrentExposure() const;
    int getActiveOrderCount() const;
    
private:
    std::map<int, Position> positions_;
    mutable std::mutex positions_mutex_;
    RiskLimits limits_;
    std::atomic<double> total_pnl_{0.0};
    std::atomic<double> current_exposure_{0.0};
    std::atomic<int> order_count_this_second_{0};
    std::chrono::steady_clock::time_point last_reset_time_;
    
    std::atomic<bool> running_{false};
    std::thread monitor_thread_;
    
    void monitorLoop();
    void resetCounters();
    void checkRiskLimits();
};

#endif