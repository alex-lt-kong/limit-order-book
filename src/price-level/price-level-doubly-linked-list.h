#ifndef PRICE_LEVEL_DOUBLY_LINKED_LIST_H
#define PRICE_LEVEL_DOUBLY_LINKED_LIST_H

#include "../order.h"
#include "../utils.h"
#include "price-level-interface.h"

#include <list>

namespace OrderBookProgrammingProblem {
class PriceLevelDoublyLinkedList
    : public IPriceLevel<PriceLevelDoublyLinkedList> {
  std::list<std::shared_ptr<Order::LimitOrder>> orders;

public:
  PriceLevelDoublyLinkedList() = default;

  void FUNC_ATTRIBUTE
  add_order_impl(const std::shared_ptr<Order::LimitOrder> &new_order) {
    if (!orders.empty()) {
      if ((*orders.begin())->price_cent != new_order->price_cent)
        throw std::invalid_argument(
            "new_order does not belong to this price level");
    }

    orders.push_back(new_order);
  }

  int FUNC_ATTRIBUTE
  update_order_impl(const std::shared_ptr<Order::LimitOrder> &new_order) {
    for (auto const &order : orders) {
      if (order->id == new_order->id) {
        order->size -= new_order->size;
        if (order->size < 0) {
          throw std::invalid_argument("new_order size is negative");
        }
        if (order->size == 0) {
          orders.remove(order);
        }
        return order->size;
      }
    }
    return -1;
  }

  [[nodiscard]] std::optional<int> FUNC_ATTRIBUTE get_level_size_impl() const {
    if (orders.empty()) {
      return std::nullopt;
    }
    int orders_size = 0;
    for (const auto &order : orders) {
      orders_size += order->size;
    }
    return orders_size;
  }

  [[nodiscard]] std::optional<int> FUNC_ATTRIBUTE get_level_price_impl() const {
    if (orders.empty()) {
      return std::nullopt;
    }
    return orders.front()->price_cent;
  }
};
} // namespace OrderBookProgrammingProblem
#endif // PRICE_LEVEL_DOUBLY_LINKED_LIST_H
