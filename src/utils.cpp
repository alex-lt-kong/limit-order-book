#include "utils.h"

namespace OrderBookProgrammingProblem {
void utils::split_string(std::string_view str, const char delimiter,
                         std::vector<std::string_view> &result) {
  result.clear();
  size_t start = 0, end;

  while ((end = str.find(delimiter, start)) != std::string_view::npos) {
    result.push_back(str.substr(start, end - start));
    start = end + 1;
  }
  result.push_back(str.substr(start)); // Add last part
}

bool utils::at_most_n_decimal_places(const std::string_view str,
                                     const size_t n) {
  const auto dot_pos = str.find('.');
  if (dot_pos == std::string::npos) {
    return true; // No decimal point
  }
  const auto decimal_places = str.size() - dot_pos - 1;
  return decimal_places <= n;
}

Order::LimitOrder utils::parse_limit_order(const std::string &str) {
  std::vector<std::string_view> parts;
  split_string(str, ' ', parts);
  if (parts.size() < 2)
    throw std::invalid_argument(
        "parts.size() < 2 (==" + std::to_string(parts.size()) + ")");
  Order::LimitOrder lo;
  lo.type = parts[1] == "A" ? Order::Type::Add : Order::Type::Reduce;

  if (lo.type == Order::Type::Add && parts.size() != 6)
    throw std::invalid_argument("Unexpected parts.size() != 6 (==" +
                                std::to_string(parts.size()) + ")");
  if (lo.type == Order::Type::Reduce && parts.size() != 4)
    throw std::invalid_argument("Unexpected parts.size() != 4 (==" +
                                std::to_string(parts.size()) + ")");

  auto [ptr, ec] = std::from_chars(
      parts[0].data(), parts[0].data() + parts[0].size(), lo.timestamp);
  if (ec != std::errc()) {
    throw std::invalid_argument("Timestamp conversion failed");
  }

  lo.id = parts[2];

  if (lo.type == Order::Type::Add) {
    lo.side = parts[3] == "S" ? Order::Side::Ask : Order::Side::Bid;
    {
      float price = 0;
      auto [ptr, ec] = std::from_chars(
          parts[4].data(), parts[4].data() + parts[4].size(), price);
      if (ec != std::errc()) {
        throw std::invalid_argument("Price conversion failed");
      }
      if (!at_most_n_decimal_places(parts[4], 2))
        throw std::invalid_argument("Price has more than 2 decimal places");
      lo.price_cent = price * 100;
    }
    {
      auto [ptr, ec] = std::from_chars(
          parts[5].data(), parts[5].data() + parts[5].size(), lo.size);
      if (ec != std::errc()) {
        throw std::invalid_argument("Size conversion failed");
      }
    }
  } else {
    auto [ptr, ec] = std::from_chars(
        parts[3].data(), parts[3].data() + parts[3].size(), lo.size);
    if (ec != std::errc()) {
      throw std::invalid_argument("Size conversion failed");
    }
  }

  return lo;
};
} // namespace OrderBookProgrammingProblem
