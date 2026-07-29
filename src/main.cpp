#include "trading_platform/trading_platform.h"
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <vector>
#include <cctype>
#include <algorithm>

class InteractiveTradingPlatform {
public:
    InteractiveTradingPlatform() : platform_(), running_(true) {}

    void run() {
        printWelcome();

        while (running_) {
            printPrompt();
            std::string input;
            std::getline(std::cin, input);

            if (input.empty()) continue;

            processCommand(input);
        }
    }

private:
    TradingPlatform platform_;
    bool running_;

    void printWelcome() {
        std::cout << "\n";
        std::cout << "============================================================\n";
        std::cout << "       MINI TRADING PLATFORM - INTERACTIVE TERMINAL          \n";
        std::cout << "============================================================\n";
        std::cout << "\n";
        printHelp();
        std::cout << "\n";
    }

    void printPrompt() {
        std::cout << "\n[Trading] > ";
    }

    void printHelp() {
        std::cout << "Available Commands:\n";
        std::cout << "------------------------------------------------------------\n";
        std::cout << "  BUY  <qty> [price]     - Place a buy order\n";
        std::cout << "                           (market order if no price)\n";
        std::cout << "  SELL <qty> [price]     - Place a sell order\n";
        std::cout << "                           (market order if no price)\n";
        std::cout << "  CANCEL <order_id>      - Cancel an existing order\n";
        std::cout << "  STATUS                 - Show order book and trade history\n";
        std::cout << "  BOOK                   - Show only order book\n";
        std::cout << "  TRADES                 - Show only trade history\n";
        std::cout << "  HELP                   - Show this help message\n";
        std::cout << "  CLEAR                  - Clear the screen\n";
        std::cout << "  EXIT / QUIT            - Exit the program\n";
        std::cout << "------------------------------------------------------------\n";
    }

    void clearScreen() {
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif
    }

    std::string toLower(const std::string& str) {
        std::string lower = str;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        return lower;
    }

    bool isNumber(const std::string& str) {
        if (str.empty()) return false;
        size_t start = 0;
        if (str[0] == '-' || str[0] == '+') start = 1;
        bool hasDecimal = false;
        for (size_t i = start; i < str.length(); ++i) {
            if (str[i] == '.') {
                if (hasDecimal) return false;
                hasDecimal = true;
            } else if (!isdigit(str[i])) {
                return false;
            }
        }
        return true;
    }

    void processCommand(const std::string& input) {
        std::vector<std::string> tokens;
        std::stringstream ss(input);
        std::string token;

        while (ss >> token) {
            tokens.push_back(token);
        }

        if (tokens.empty()) return;

        std::string command = toLower(tokens[0]);

        if (command == "exit" || command == "quit") {
            std::cout << "\nGoodbye!\n";
            running_ = false;
            return;
        }
        else if (command == "clear") {
            clearScreen();
            printWelcome();
            return;
        }
        else if (command == "help") {
            printHelp();
            return;
        }
        else if (command == "status") {
            platform_.printStatus();
            return;
        }
        else if (command == "book") {
            platform_.printOrderBook();
            return;
        }
        else if (command == "trades") {
            platform_.printTradeHistory();
            return;
        }
        else if (command == "buy" || command == "sell") {
            handleOrder(tokens);
            return;
        }
        else if (command == "cancel") {
            handleCancel(tokens);
            return;
        }
        else {
            std::cout << "Unknown command: " << command << "\n";
            std::cout << "Type 'HELP' for available commands\n";
        }
    }

    void handleOrder(const std::vector<std::string>& tokens) {
        if (tokens.size() < 2) {
            std::cout << "Usage: " << tokens[0] << " <quantity> [price]\n";
            return;
        }

        std::string sideStr = tokens[0];
        OrderSide side = (toLower(sideStr) == "buy") ? OrderSide::BUY : OrderSide::SELL;

        int quantity;
        if (!isNumber(tokens[1]) || tokens[1].find('.') != std::string::npos) {
            std::cout << "ERROR: Quantity must be a whole number\n";
            return;
        }
        quantity = std::stoi(tokens[1]);

        if (quantity <= 0) {
            std::cout << "ERROR: Quantity must be positive\n";
            return;
        }

        double price = 0.0;
        OrderType type = OrderType::MARKET;

        if (tokens.size() >= 3) {
            if (!isNumber(tokens[2])) {
                std::cout << "ERROR: Invalid price format\n";
                return;
            }
            price = std::stod(tokens[2]);
            if (price <= 0) {
                std::cout << "ERROR: Price must be positive\n";
                return;
            }
            type = OrderType::LIMIT;
        }

        int orderId = platform_.placeOrder(side, type, quantity, price);

        if (orderId != -1) {
            std::cout << "[SUCCESS] Order placed! Order ID: " << orderId << "\n";
        } else {
            std::cout << "[FAILED] Could not place order. Check parameters.\n";
        }
    }

    void handleCancel(const std::vector<std::string>& tokens) {
        if (tokens.size() < 2) {
            std::cout << "Usage: CANCEL <order_id>\n";
            return;
        }

        if (!isNumber(tokens[1]) || tokens[1].find('.') != std::string::npos) {
            std::cout << "ERROR: Order ID must be a whole number\n";
            return;
        }

        int orderId = std::stoi(tokens[1]);

        if (orderId <= 0) {
            std::cout << "ERROR: Order ID must be positive\n";
            return;
        }

        bool cancelled = platform_.cancelOrder(orderId);

        if (cancelled) {
            std::cout << "[SUCCESS] Order " << orderId << " cancelled!\n";
        } else {
            std::cout << "[FAILED] Cannot cancel order " << orderId << ". ";
            std::cout << "Order may not exist or may already be filled.\n";
        }
    }
};

int main() {
    InteractiveTradingPlatform interactive;
    interactive.run();
    return 0;
}