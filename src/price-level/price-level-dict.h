#ifndef PRICE_LEVEL_DICT_H
#define PRICE_LEVEL_DICT_H

#include "../order.h"
#include "../utils.h"
#include "price-level-interface.h"

#include <memory>
#include <unordered_map>

namespace OrderBookProgrammingProblem {
class PriceLevelDict : public IPriceLevel<PriceLevelDict> {
  std::unordered_map<std::string, std::shared_ptr<Order::LimitOrder>> orders;

public:
  PriceLevelDict() = default;

  void FUNC_ATTRIBUTE
  add_order_impl(const std::shared_ptr<Order::LimitOrder> &new_order) {
    if (!orders.empty()) {
      if (orders.begin()->second->price_cent != new_order->price_cent)
        throw std::invalid_argument(
            "new_order does not belong to this price level");
    }

    orders[new_order->id] = new_order;
  }

  int FUNC_ATTRIBUTE
  update_order_impl(const std::shared_ptr<Order::LimitOrder> &new_order) {
    for (const auto &[k, val] : orders) {
      if (k == new_order->id) {
        val->size -= new_order->size;
        if (val->size < 0) {
          throw std::invalid_argument("new_order size is negative");
        }
        if (val->size == 0) {
          orders.erase(k);
        }
        return val->size;
      }
    }
    return -1;
  }

  [[nodiscard]] std::optional<int> FUNC_ATTRIBUTE get_level_size_impl() const {
    if (orders.empty()) {
      return std::nullopt;
    }
    int orders_size = 0;
    for (const auto &[k, val] : orders) {
      orders_size += val->size;
    }
    return orders_size;
  }

  [[nodiscard]] std::optional<int> FUNC_ATTRIBUTE get_level_price_impl() const {
    if (orders.empty()) {
      return std::nullopt;
    }
    return orders.begin()->second->price_cent;
  }
};
} // namespace OrderBookProgrammingProblem
#endif // PRICE_LEVEL_DICT_H
