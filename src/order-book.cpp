#include "order-book.h"

#include <ranges>

namespace OrderBookProgrammingProblem {

void OrderBook::add_order(const Order::LimitOrder &new_order) {
  const auto order_ptr = std::make_shared<Order::LimitOrder>(new_order);
  if (new_order.type == Order::Type::Add) {
    auto &prices = new_order.side == Order::Side::Ask ? ask_prices : bid_prices;

    if (prices.size() <= new_order.price_cent) {
      prices.resize(new_order.price_cent + 1);
    }

    prices[new_order.price_cent].push_back(order_ptr);
    order_by_id[new_order.id] = order_ptr;
    return;
  }

  const auto existing_order = order_by_id[new_order.id];
  if (!existing_order) {
    throw std::invalid_argument("Order not found");
  }

  auto update_order = [&](auto &prices) {
    for (auto const &order : prices[existing_order->price_cent]) {
      if (order->id == new_order.id) {
        order->size -= new_order.size;
        if (order->size < 0) {
          throw std::invalid_argument("new_order size is negative");
        }
        if (order->size == 0) {
          prices[existing_order->price_cent].remove(order);
          order_by_id.erase(new_order.id);
        }
        return;
      }
    }
  };

  if (existing_order->side == Order::Side::Ask) {
    update_order(ask_prices);
  } else {
    update_order(bid_prices);
  }
}

std::string OrderBook::to_string() {
  auto get_repr_of_one_side = [](const std::string &label,
                                 auto &price_levels) -> std::string {
    std::string result = label + ":\n";
    int accu_volume = 0, accu_size = 0;

    for (const auto &price_level : std::views::reverse(price_levels)) {
      if (!price_level.empty()) {
        result += std::format("Price: {:>5.02f}, ",
                              price_level.front()->price_cent / 100.0);
        int level_size = 0;
        std::string orders_detail = "{ ";
        for (const auto &order : price_level) {
          level_size += order->size;
          orders_detail += std::format("{{ id: {:>3}, size: {:>3} }}, ",
                                       order->id, order->size);
        }
        orders_detail.pop_back();
        orders_detail.pop_back();
        orders_detail += " }";
        accu_size += level_size;
        accu_volume += level_size * price_level.front()->price_cent;
        result += std::format(
            "Size: {:>5}, AccuSize: {:>5}, AccuVolume: {:>8}, Orders: {}\n",
            level_size, accu_size, accu_volume, orders_detail);
      }
    }

    return result;
  };
  return get_repr_of_one_side("Ask", ask_prices) +
         get_repr_of_one_side("Bid", bid_prices);
}
} // namespace OrderBookProgrammingProblem