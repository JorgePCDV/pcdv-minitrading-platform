#include "trading_platform/order.h"

Order::Order(int id, OrderSide side, OrderType type, int quantity, double price)
    : id_(id), side_(side), type_(type), original_quantity_(quantity),
      remaining_quantity_(quantity), price_(price), status_(OrderStatus::PENDING) {}

int Order::getId() const { return id_; }
OrderSide Order::getSide() const { return side_; }
OrderType Order::getType() const { return type_; }
int Order::getRemainingQuantity() const { return remaining_quantity_; }
double Order::getPrice() const { return price_; }
OrderStatus Order::getStatus() const { return status_; }

void Order::fill(int quantity) {
    if (quantity > remaining_quantity_) return;
    remaining_quantity_ -= quantity;
    if (remaining_quantity_ == 0) {
        status_ = OrderStatus::FILLED;
    } else {
        status_ = OrderStatus::PARTIALLY_FILLED;
    }
}

void Order::cancel() {
    if (status_ == OrderStatus::PENDING || status_ == OrderStatus::PARTIALLY_FILLED) {
        status_ = OrderStatus::CANCELLED;
    }
}

std::string Order::toString() const {
    std::stringstream ss;
    ss << "Order{id:" << id_ << ", side:" << (side_ == OrderSide::BUY ? "BUY" : "SELL")
       << ", type:" << (type_ == OrderType::MARKET ? "MARKET" : "LIMIT")
       << ", remaining:" << remaining_quantity_ << "/" << original_quantity_
       << ", price:" << price_ << ", status:" << static_cast<int>(status_) << "}";
    return ss.str();
}