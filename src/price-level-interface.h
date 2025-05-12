#ifndef PRICE_LEVEL_INTERFACE_H
#define PRICE_LEVEL_INTERFACE_H

#include "order.h"

#include <list>

namespace OrderBookProgrammingProblem {
template <typename T> class IPriceLevel {

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
  static void ctor_impl() { throw std::logic_error("Not implemented"); }

public:
  void __attribute__((noinline))
  add_order(const std::shared_ptr<Order::LimitOrder> &new_order) {
    return static_cast<T *>(this)->add_order_impl(new_order);
  }

  int __attribute__((noinline))
  update_order(const std::shared_ptr<Order::LimitOrder> &new_order) {
    return static_cast<T *>(this)->update_order_impl(new_order);
  }

  std::optional<int> __attribute__((noinline)) get_level_size() {
    return static_cast<T *>(this)->get_level_size_impl();
  }

  std::optional<int> __attribute__((noinline)) get_level_price() {
    return static_cast<T *>(this)->get_level_price_impl();
  }
};
} // namespace OrderBookProgrammingProblem
#endif // PRICE_LEVEL_INTERFACE_H
