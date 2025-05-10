#ifndef ORDER_BOOK_H
#define ORDER_BOOK_H

#include "order.h"

#include <list>
#include <memory>
#include <unordered_map>
#include <vector>

namespace OrderBookProgrammingProblem {
class OrderBook {
public:
  std::vector<std::list<std::shared_ptr<Order::LimitOrder>>> ask_prices;
  std::vector<std::list<std::shared_ptr<Order::LimitOrder>>> bid_prices;
  std::unordered_map<std::string, std::shared_ptr<Order::LimitOrder>>
      order_by_id;

  void add_order(const Order::LimitOrder &new_order);

  std::string to_string();
};
} // namespace OrderBookProgrammingProblem

#endif // ORDER_BOOK_H
