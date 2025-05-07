#ifndef UTILS_H
#define UTILS_H

#include "order.h"

#include <string_view>
#include <vector>

namespace OrderBookProgrammingProblem {
class utils {
  static void split_string(std::string_view str, char delimiter,
                           std::vector<std::string_view> &result);

public:
  static bool at_most_n_decimal_places(std::string_view str, size_t n);

  static Order::LimitOrder parse_limit_order(const std::string &);
};
} // namespace OrderBookProgrammingProblem

#endif // UTILS_H
