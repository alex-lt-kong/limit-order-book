#include "order-book.h"
#include "utils.h"

#include <climits>
#include <iostream>
#include <ranges>
#include <string>

namespace Problem = OrderBookProgrammingProblem;
namespace Order = Problem::Order;

template <typename View>
std::optional<int> get_cost_cent(
    const std::vector<std::list<std::shared_ptr<Order::LimitOrder>>> &levels,
    View transform, int target_size) {
  int cost_cent = 0;
  for (const auto &level : transform(levels)) {
    for (const auto &entry : std::views::reverse(level)) {
      if (target_size > entry->size) {
        target_size -= entry->size;
        cost_cent += entry->size * entry->price_cent;
      } else {
        cost_cent += target_size * entry->price_cent;
        target_size = 0;
        break;
      }
    }
    if (target_size == 0)
      break;
  }
  if (target_size > 0)
    return std::nullopt;
  return cost_cent;
}

std::optional<int> get_sell_cost_cent(
    const std::vector<std::list<std::shared_ptr<Order::LimitOrder>>> &levels,
    const int target_size) {
  // Think about it, for get_cost_cent, if I want to complete its type,
  // like get_cost_cent<T>(levels, std::views::reverse, target_size);
  // what would T be?
  return get_cost_cent(levels, std::views::reverse, target_size);
}

std::optional<int> get_buy_cost_cent(
    const std::vector<std::list<std::shared_ptr<Order::LimitOrder>>> &levels,
    const int target_size) {
  return get_cost_cent<decltype(std::views::all)>(levels, std::views::all,
                                                  target_size);
}

void parse_cin() {
  Problem::OrderBook order_book;
  std::string line;
  int min_price = INT_MAX;
  int max_price = INT_MIN;
  std::optional<int> sell_cost_cent = std::nullopt;
  std::optional<int> buy_cost_cent = std::nullopt;
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
    std::cerr << lo << " (min_price: " << min_price
              << ", max_price: " << max_price << ")\n";
    std::cerr << "OrderBook:\n" << order_book.to_string() << "\n";
    auto new_sell_cost_cent = get_sell_cost_cent(order_book.bids, 200);
    if (new_sell_cost_cent != sell_cost_cent) {
      std::string line = std::to_string(lo.timestamp) + " S ";
      if (new_sell_cost_cent.has_value())
        line += std::to_string(new_sell_cost_cent.value() / 100.0);
      else
        line += "NA";
      std::cout << line << "\n";
      std::cerr << line << "\n";
      sell_cost_cent = new_sell_cost_cent;
    }
    auto new_buy_cost_cent = get_buy_cost_cent(order_book.asks, 200);
    std::cerr << "new_buy_cost_cent: " << new_buy_cost_cent.value_or(-1)
              << "\n";
    if (new_buy_cost_cent != buy_cost_cent) {
      std::string line = std::to_string(lo.timestamp) + " B ";
      if (new_buy_cost_cent.has_value())
        line += std::to_string(new_buy_cost_cent.value() / 100.0);
      else
        line += "NA";
      std::cout << line << "\n";
      std::cerr << line << "\n";
      buy_cost_cent = new_buy_cost_cent;
    }
  }
}

int main(int argc, char *argv[]) {
  parse_cin();
  std::cerr << argv[0] << " exited gracefully" << std::endl;
  return 0;
}