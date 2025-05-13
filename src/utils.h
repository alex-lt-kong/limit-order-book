#ifndef UTILS_H
#define UTILS_H

#include "order.h"

#include <string_view>
#include <vector>

// #define FUNC_ATTRIBUTE
#define FUNC_ATTRIBUTE                                                         \
  __attribute__((noinline)) /* Disable inline to show stack frames for         \
                               benchmarking purpose */

#if defined(BENCHMARK_PERFORMANCE) && (BENCHMARK_PERFORMANCE == 1)
constexpr bool benchmark_performance = true;
#else
constexpr bool benchmark_performance = false;
#endif

namespace OrderBookProgrammingProblem {
class Utils {
private:
  std::vector<std::string_view> order_line_parts;

  static void split_string(std::string_view str, char delimiter,
                           std::vector<std::string_view> &result);

public:
  static bool at_most_n_decimal_places(std::string_view str, size_t n);

  Order::LimitOrder parse_limit_order(const std::string &);
};
} // namespace OrderBookProgrammingProblem

#endif // UTILS_H
