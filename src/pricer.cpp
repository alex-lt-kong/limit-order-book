#include "order-book-array.h"
#include "utils.h"

#include <iostream>
#include <ranges>
#include <string>

#if defined(BENCHMARK_PERFORMANCE) && (BENCHMARK_PERFORMANCE == 1)
constexpr bool benchmark_performance = true;
#else
constexpr bool benchmark_performance = false;
#endif

namespace Problem = OrderBookProgrammingProblem;
namespace Order = Problem::Order;

bool update_previous_cost_cent(const std::optional<int> new_cost_cent,
                               std::optional<int> &previous_cost_cent) {
  if (new_cost_cent != previous_cost_cent) {
    previous_cost_cent = new_cost_cent;
    return true;
  }
  return false;
}

void __attribute__((noinline))
print_new_cost(const Order::LimitOrder &lo,
               const std::optional<int> new_cost_cent, const bool is_sell) {
  std::string line = std::to_string(lo.timestamp) + (is_sell ? " S " : " B ");
  if (new_cost_cent.has_value())
    line += std::format("{:.2f}", new_cost_cent.value() / 100.0);
  else
    line += "NA";
  std::cout << line << "\n";
  std::cerr << line << "\n\n";
}

int main(const int argc, char *argv[]) {
  int target_size = 200;
  if (argc == 2) {
    target_size = std::stoi(argv[1]);
  }
  if constexpr (!benchmark_performance)
    std::cerr << argv[0] << " started with target size: " << target_size
        << std::endl;
  auto order_book = Problem::OrderBookArray();
  Problem::Utils utils;
  std::string in_line;
  std::optional<Order::LimitOrder> prev_lo;
  size_t price_change_count = 0;
  // buy/sell here is from pricer's perspective
  std::optional<int> sell_cost_cent = std::nullopt;
  std::optional<int> buy_cost_cent = std::nullopt;
  // Asked Microsoft Copilot and confirmed by checking source code,
  // std::getline() reuses in_line, it wont allocate new std::string each time
  while (std::getline(std::cin, in_line)) {
    const auto lo = utils.parse_limit_order(in_line);
    if (prev_lo.has_value()) {
      if (lo.timestamp < prev_lo->timestamp)
        throw std::invalid_argument("timestamp not monotonically increasing");
    }
    prev_lo = lo;
    order_book.add_order(lo);
    if constexpr (!benchmark_performance) {
      std::cerr << "LimitOrder: " << lo << "\n";
      std::cerr << "OrderBook:\n" << order_book.to_string() << "\n";
    }
    const auto new_sell_cost_cent =
        order_book.get_pricer_sell_cost_cent(target_size);
    if (update_previous_cost_cent(new_sell_cost_cent, sell_cost_cent)) {
      if constexpr (!benchmark_performance)
        print_new_cost(lo, new_sell_cost_cent, true);
      else
        ++price_change_count;
    }
    const auto new_buy_cost_cent =
        order_book.get_pricer_buy_cost_cent(target_size);
    if (update_previous_cost_cent(new_buy_cost_cent, buy_cost_cent)) {
      if constexpr (!benchmark_performance)
        print_new_cost(lo, new_buy_cost_cent, false);
      else
        ++price_change_count;
    }
  }
  // Even with benchmark_performance == true we'd better print something
  // meaningful at the end...just in case the compiler is super smart and
  // optimizes everything away
  std::cerr << argv[0] << " exited gracefully, price changed "
      << price_change_count << " time(s)" << std::endl;
  return 0;
}
