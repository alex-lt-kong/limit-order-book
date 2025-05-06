#include "order.h"

#include <print>

namespace Order = OrderBookProgrammingProblem::Order;

int main() {
    Order::LimitOrder lo;
    lo.timestamp = 1234567890;
    lo.type = Order::Type::Add;
    lo.price = 3.14;
    lo.size = 123;
    lo.id = "0xdeadbeef";
    std::cout << lo << std::endl;
    return 0;
}
