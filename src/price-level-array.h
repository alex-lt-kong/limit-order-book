#ifndef PRICE_LEVEL_ARRAY_H
#define PRICE_LEVEL_ARRAY_H

#include <algorithm>

#include "utils.h"
#include "order.h"
#include "price-level-interface.h"

#include <memory>
#include <ranges>
#include <format>

namespace OrderBookProgrammingProblem {
  class PriceLevelArray : public IPriceLevel<PriceLevelArray> {
    std::vector<std::shared_ptr<Order::LimitOrder> > orders;

  public:
    void FUNC_ATTRIBUTE add_order_impl(const std::shared_ptr<Order::LimitOrder> &new_order) {
      if (!orders.empty()) {
        if ((*orders.begin())->price_cent != new_order->price_cent)
          throw std::invalid_argument(
            "new_order does not belong to this price level");
      }

      orders.push_back(new_order);
    }

    int FUNC_ATTRIBUTE update_order_impl(const std::shared_ptr<Order::LimitOrder> &new_order) {
      for (auto i = 0; i < orders.size(); ++i) {
        if (orders[i]->id == new_order->id) {
          orders[i]->size -= new_order->size;
          if (orders[i]->size < 0) {
            throw std::invalid_argument("new_order size is negative");
          }
          if (orders[i]->size == 0) {
            std::swap(orders[i], orders.back());
            orders.pop_back();
          }
          return orders[i]->size;
        }
      }
      return -1;
    }

    [[nodiscard]] std::optional<int> FUNC_ATTRIBUTE get_level_size_impl() const {
      if (orders.empty()) {
        return std::nullopt;
      }
      int orders_size = 0;
      for (const auto &order: orders) {
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

    [[nodiscard]] std::string to_string_impl() const {
      if (orders.empty()) { return ""; }
      std::string repr = "{";
      for (const auto &order: orders) {
        repr += std::format(" {{ id: {:>5}, size: {:>4} }}, ", order->id, order->size);
      }
      repr.pop_back();
      repr.pop_back();
      return repr + " }";
    }

    // Custom three-way comparison
    auto operator<=>(const PriceLevelArray &other) const {
      /*if (other.orders.empty() && orders.empty()) {
        throw std::logic_error("other.orders.empty() || orders.empty()");
      }
      if (other.orders.empty()) {
        return -1;
      }*/
      return orders.front()->price_cent <=> other.orders.front()->price_cent;
    }

    bool operator==(const PriceLevelArray &other) const {
      /*
      if (other.orders.empty() || orders.empty()) {
        throw std::logic_error("other.orders.empty() || orders.empty()");
      }*/
      return orders.front()->price_cent == other.orders.front()->price_cent;
    }
  };
} // namespace OrderBookProgrammingProblem
#endif // PRICE_LEVEL_ARRAY_H
