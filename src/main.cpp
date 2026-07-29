#include "trading_platform/multi_threaded_platform.h"
#include <iostream>
#include <string>
#include <sstream>

int main() {
    MultiThreadedPlatform platform;
    platform.start();

    std::cout << "\n=== Multi-Threaded Trading Platform ===\n";
    std::cout << "Commands: BUY <qty> <price> | SELL <qty> <price> | STATUS | EXIT\n\n";

    std::string line;
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, line);

        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string cmd;
        ss >> cmd;

        if (cmd == "EXIT" || cmd == "QUIT") {
            break;
        } else if (cmd == "STATUS") {
            platform.printStatus();
        } else if (cmd == "BUY" || cmd == "SELL") {
            int qty;
            double price = 0;
            ss >> qty;
            if (!ss.eof()) ss >> price;

            OrderSide side = (cmd == "BUY") ? OrderSide::BUY : OrderSide::SELL;
            OrderType type = (price > 0) ? OrderType::LIMIT : OrderType::MARKET;

            int id = platform.placeOrderAsync(side, type, qty, price);
            if (id != -1) {
                std::cout << "Order " << id << " submitted (async)\n";
            } else {
                std::cout << "Order rejected\n";
            }
        } else {
            std::cout << "Unknown command\n";
        }
    }

    platform.stop();
    return 0;
}