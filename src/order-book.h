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
  std::vector<std::list<std::shared_ptr<Order::LimitOrder>>> asks;
  std::vector<std::list<std::shared_ptr<Order::LimitOrder>>> bids;
  std::unordered_map<std::string, std::shared_ptr<Order::LimitOrder>>
      order_by_id;

  void add_order(Order::LimitOrder order);

  std::string to_string() const;
};
} // namespace OrderBookProgrammingProblem

#endif // ORDER_BOOK_H
