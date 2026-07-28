#include "trading_platform/trade.h"

Trade::Trade(int buyId, int sellId, int qty, double p)
    : buy_order_id(buyId), sell_order_id(sellId), quantity(qty), price(p),
      timestamp(std::chrono::system_clock::now()) {}

std::string Trade::toString() const {
    auto time = std::chrono::system_clock::to_time_t(timestamp);
    std::stringstream ss;
    ss << "Trade{ buy:" << buy_order_id << ", sell:" << sell_order_id 
       << ", qty:" << quantity << ", price:" << std::fixed << std::setprecision(2) << price
       << ", time:" << std::put_time(std::localtime(&time), "%H:%M:%S") << "}";
    return ss.str();
}