#ifndef ORDER_H
#define ORDER_H

#include <string>
#include <chrono>
#include <sstream>
#include <iomanip>

enum class OrderSide { BUY, SELL };
enum class OrderType { MARKET, LIMIT };
enum class OrderStatus { PENDING, FILLED, PARTIALLY_FILLED, CANCELLED };

class Order {
public:
    Order(int id, OrderSide side, OrderType type, int quantity, double price = 0.0);
    
    int getId() const;
    OrderSide getSide() const;
    OrderType getType() const;
    int getRemainingQuantity() const;
    double getPrice() const;
    OrderStatus getStatus() const;
    
    void fill(int quantity);
    void cancel();
    std::string toString() const;
    
private:
    int id_;
    OrderSide side_;
    OrderType type_;
    int original_quantity_;
    int remaining_quantity_;
    double price_;
    OrderStatus status_;
};

#endif