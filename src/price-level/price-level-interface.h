#ifndef PRICE_LEVEL_INTERFACE_H
#define PRICE_LEVEL_INTERFACE_H

#include "../order.h"

#include <memory>

namespace OrderBookProgrammingProblem {
  template<typename T>
  class IPriceLevel {
    static void add_order_impl(const std::shared_ptr<Order::LimitOrder> &) {
      throw std::logic_error("Not implemented");
    }

    static int update_order_impl(const std::shared_ptr<Order::LimitOrder> &) {
      throw std::logic_error("Not implemented");
    }

    static std::optional<int> get_level_size_impl() {
      throw std::logic_error("Not implemented");
    }

    static std::optional<int> get_level_price_impl() {
      throw std::logic_error("Not implemented");
    }

    static std::string to_string_impl() {
      throw std::logic_error("Not implemented");
    }

  public:
    void add_order(const std::shared_ptr<Order::LimitOrder> &new_order) {
      return static_cast<T *>(this)->add_order_impl(new_order);
    }

    int update_order(const std::shared_ptr<Order::LimitOrder> &new_order) {
      return static_cast<T *>(this)->update_order_impl(new_order);
    }

    std::optional<int> get_level_size() {
      return static_cast<T *>(this)->get_level_size_impl();
    }

    std::optional<int> get_level_price() {
      return static_cast<T *>(this)->get_level_price_impl();
    }

    std::string to_string() { return static_cast<T *>(this)->to_string_impl(); }
  };
} // namespace OrderBookProgrammingProblem
#endif // PRICE_LEVEL_INTERFACE_H
