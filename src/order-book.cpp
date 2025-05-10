#include "order-book.h"

#include <ranges>

namespace OrderBookProgrammingProblem {

void OrderBook::add_order(const Order::LimitOrder &order) {
  const auto order_ptr = std::make_shared<Order::LimitOrder>(order);
  if (order.type == Order::Type::Add) {
    if (order.side == Order::Side::Ask) {
      if (ask_prices.size() <= order.price_cent) {
        ask_prices.resize(order.price_cent + 1);
      }
      ask_prices[order.price_cent].push_back(order_ptr);
    } else {
      if (bid_prices.size() <= order.price_cent) {
        bid_prices.resize(order.price_cent + 1);
      }
      bid_prices[order.price_cent].push_back(order_ptr);
    }
    order_by_id[order.id] = order_ptr;
    return;
  }

  const auto existing_order = order_by_id[order.id];
  if (!existing_order) {
    throw std::invalid_argument("Order not found");
  }

  if (existing_order->side == Order::Side::Ask) {
    for (auto const &ask : ask_prices[existing_order->price_cent]) {
      if (ask->id == order.id) {
        ask->size -= order.size;
        if (ask->size < 0)
          throw std::invalid_argument("ask size is negative");
        if (ask->size == 0) {
          ask_prices[existing_order->price_cent].remove(ask);
          order_by_id.erase(order.id);
        }
        return;
      }
    }
  } else {
    for (const auto &bid : bid_prices[existing_order->price_cent]) {
      if (bid->id == order.id) {
        bid->size -= order.size;
        if (bid->size < 0)
          throw std::invalid_argument("bid size is negative");
        if (bid->size == 0) {
          bid_prices[existing_order->price_cent].remove(bid);
          order_by_id.erase(order.id);
        }
        return;
      }
    }
  }
}

std::string OrderBook::to_string() const {
  std::string order_book_repr;
  int accu_volume = 0;
  int accu_size = 0;
  order_book_repr += "Ask:\n";
  for (const auto &ask_price : std::views::reverse(ask_prices)) {
    if (!ask_price.empty()) {
      order_book_repr += std::format("Price: {:>5.02f}, ",
                                     ask_price.front()->price_cent / 100.0);
      int level_size = 0;
      for (const auto &order : ask_price) {
        level_size += order->size;
      }
      accu_size += level_size;
      accu_volume += level_size * ask_price.front()->price_cent;
      order_book_repr +=
          std::format("Size: {:>5}, AccuSize: {:>5}, AccuVolume: {:>8}\n",
                      level_size, accu_size, accu_volume);
    }
  }

  accu_volume = 0;
  accu_size = 0;
  order_book_repr += "Bid:\n";
  for (const auto &bid_price : std::views::reverse(bid_prices)) {
    if (!bid_price.empty()) {
      order_book_repr += std::format("Price: {:>5.02f}, ",
                                     bid_price.front()->price_cent / 100.0);
      int level_size = 0;
      for (const auto &order : bid_price) {
        level_size += order->size;
      }
      accu_size += level_size;
      accu_volume += level_size * bid_price.front()->price_cent;
      order_book_repr +=
          std::format("Size: {:>5}, AccuSize: {:>5}, AccuVolume: {:>8}\n",
                      level_size, accu_size, accu_volume);
    }
  }
  return order_book_repr;
}
} // namespace OrderBookProgrammingProblem