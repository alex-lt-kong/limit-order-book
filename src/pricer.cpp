#include "utils.h"

#include <climits>
#include <iostream>
#include <string>

namespace Problem = OrderBookProgrammingProblem;
namespace Order = Problem::Order;

void parse_cin() {
  std::string line;
  int min_price = INT_MAX;
  int max_price = INT_MIN;
  while (std::getline(std::cin, line)) {
    auto lo = Problem::utils::parse_limit_order(line);
    // auto changed = false;
    if (lo.type == Order::Type::Add) {
      if (lo.price_cent < min_price) {
        min_price = lo.price_cent;
      }
      if (lo.price_cent > max_price) {
        max_price = lo.price_cent;
      }
    }
    std::cout << "line: " << line << ", lo: " << lo
              << " (min_price: " << min_price << ", max_price: " << max_price
              << ")\n";
  }
}

int main() {
  parse_cin();
  return 0;
}