#include "order-book.h"
#include "utils.h"

#include <climits>
#include <iostream>
#include <string>

namespace Problem = OrderBookProgrammingProblem;
namespace Order = Problem::Order;

void parse_cin() {
  Problem::OrderBook order_book;
  std::string line;
  int min_price = INT_MAX;
  int max_price = INT_MIN;
  while (std::getline(std::cin, line)) {
    auto lo = Problem::utils::parse_limit_order(line);
    order_book.add_order(lo);
    // auto changed = false;
    if (lo.type == Order::Type::Add) {
      if (lo.price_cent < min_price) {
        min_price = lo.price_cent;
      }
      if (lo.price_cent > max_price) {
        max_price = lo.price_cent;
      }
    }
    std::cout << lo << " (min_price: " << min_price
              << ", max_price: " << max_price << ")\n";
    std::cout << "OrderBook:\n" << order_book.to_string() << "\n";
  }
}

int main(int argc, char *argv[]) {
  parse_cin();
  std::cout << argv[0] << " exited gracefully" << std::endl;
  return 0;
}