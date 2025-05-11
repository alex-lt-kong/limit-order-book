#include "utils.h"

namespace OrderBookProgrammingProblem {
void Utils::split_string(std::string_view str, const char delimiter,
                         std::vector<std::string_view> &result) {
  result.clear();
  size_t start = 0, end;

  while ((end = str.find(delimiter, start)) != std::string_view::npos) {
    result.push_back(str.substr(start, end - start));
    start = end + 1;
  }
  result.push_back(str.substr(start)); // Add last part
}

bool Utils::at_most_n_decimal_places(const std::string_view str,
                                     const size_t n) {
  const auto dot_pos = str.find('.');
  if (dot_pos == std::string::npos) {
    return true; // No decimal point
  }
  const auto decimal_places = str.size() - dot_pos - 1;
  return decimal_places <= n;
}

Order::LimitOrder Utils::parse_limit_order(const std::string &str) {
  split_string(str, ' ', order_line_parts);
  if (order_line_parts.size() < 2)
    throw std::invalid_argument("order_line_parts.size() < 2 (==" +
                                std::to_string(order_line_parts.size()) + ")");
  Order::LimitOrder lo;
  lo.type = order_line_parts[1] == "A" ? Order::Type::Add : Order::Type::Reduce;

  if (lo.type == Order::Type::Add && order_line_parts.size() != 6)
    throw std::invalid_argument("Unexpected order_line_parts.size() != 6 (==" +
                                std::to_string(order_line_parts.size()) + ")");
  if (lo.type == Order::Type::Reduce && order_line_parts.size() != 4)
    throw std::invalid_argument("Unexpected order_line_parts.size() != 4 (==" +
                                std::to_string(order_line_parts.size()) + ")");

  auto [ptr, ec] = std::from_chars(
      order_line_parts[0].data(),
      order_line_parts[0].data() + order_line_parts[0].size(), lo.timestamp);
  if (ec != std::errc()) {
    throw std::invalid_argument("Timestamp conversion failed");
  }

  lo.id = order_line_parts[2];

  if (lo.type == Order::Type::Add) {
    lo.side = order_line_parts[3] == "S" ? Order::Side::Ask : Order::Side::Bid;
    {
      float price = 0;
      auto [ptr, ec] = std::from_chars(
          order_line_parts[4].data(),
          order_line_parts[4].data() + order_line_parts[4].size(), price);
      if (ec != std::errc()) {
        throw std::invalid_argument("Price conversion failed");
      }
      if (!at_most_n_decimal_places(order_line_parts[4], 2))
        throw std::invalid_argument("Price has more than 2 decimal places");
      lo.price_cent = price * 100;
    }
    {
      auto [ptr, ec] = std::from_chars(
          order_line_parts[5].data(),
          order_line_parts[5].data() + order_line_parts[5].size(), lo.size);
      if (ec != std::errc()) {
        throw std::invalid_argument("Size conversion failed");
      }
    }
  } else {
    auto [ptr, ec] = std::from_chars(
        order_line_parts[3].data(),
        order_line_parts[3].data() + order_line_parts[3].size(), lo.size);
    if (ec != std::errc()) {
      throw std::invalid_argument("Size conversion failed");
    }
  }

  return lo;
};
} // namespace OrderBookProgrammingProblem
