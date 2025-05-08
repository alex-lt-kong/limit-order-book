#include "order-book.h"

#include <ranges>

namespace OrderBookProgrammingProblem {

void OrderBook::add_order(Order::LimitOrder order) {
  const auto order_ptr = std::make_shared<Order::LimitOrder>(order);
  if (order.type == Order::Type::Add) {
    if (order.side == Order::Side::Ask) {
      if (asks.size() <= order.price_cent) {
        asks.resize(order.price_cent + 1);
      }
      asks[order.price_cent].push_back(order_ptr);
    } else {
      if (bids.size() <= order.price_cent) {
        bids.resize(order.price_cent + 1);
      }
      bids[order.price_cent].push_back(order_ptr);
    }
    order_by_id[order.id] = order_ptr;
    return;
  }

  const auto existing_order = order_by_id[order.id];
  if (existing_order->side == Order::Side::Ask) {
    for (auto &ask : asks[existing_order->price_cent]) {
      if (ask->id == order.id) {
        ask->size -= order.size;
        if (ask->size < 0)
          throw std::invalid_argument("ask size is negative");
        if (ask->size == 0) {
          asks[existing_order->price_cent].remove(ask);
          order_by_id.erase(order.id);
        }
        return;
      }
    }
  }

  for (const auto &bid : bids[existing_order->price_cent]) {
    if (bid->id == order.id) {
      bid->size -= order.size;
      if (bid->size < 0)
        throw std::invalid_argument("bid size is negative");
      if (bid->size == 0) {
        bids[existing_order->price_cent].remove(bid);
        order_by_id.erase(order.id);
      }
      return;
    }
  }
}

std::string OrderBook::to_string() const {
  std::string order_book_repr;
  order_book_repr += "Ask:\n";
  for (const auto &ask : std::views::reverse(asks)) {
    if (ask.size() > 0) {
      order_book_repr += "Price: ";
      order_book_repr += std::to_string(ask.front()->price_cent / 100.0);
      order_book_repr += ", Size: ";
      size_t level_size = 0;
      for (const auto &order : ask) {
        level_size += order->size;
      }
      order_book_repr += std::to_string(level_size) + "\n";
    }
  }
  order_book_repr += "Bid:\n";
  for (const auto &bid : std::views::reverse(bids)) {
    if (bid.size() > 0) {
      order_book_repr += "Price: ";
      order_book_repr += std::to_string(bid.front()->price_cent / 100.0);
      order_book_repr += ", Size: ";
      size_t level_size = 0;
      for (const auto &order : bid) {
        level_size += order->size;
      }
      order_book_repr += std::to_string(level_size) + "\n";
    }
  }
  return order_book_repr;
}
} // namespace OrderBookProgrammingProblem