#ifndef ORDER_H
#define ORDER_H

#include <format>
#include <iostream>
#include <string>

namespace OrderBookProgrammingProblem {
  namespace Order {
    enum struct Side : char { Bid = 'B', Ask = 'A' };

    enum struct Type : char { Add = 'A', Reduce = 'R' };

    struct LimitOrder {
      uint64_t timestamp = 0;
      Type type = Type::Add;
      std::string id;
      Side side = Side::Bid;
      int price_cent = 0;
      int size = 0;

      // This is for testing data structures, dont remove it for now
      LimitOrder(const int price) : price_cent(price) {
      }

      LimitOrder() = default;

      // Custom three-way comparison
      auto operator<=>(const LimitOrder &other) const {
        return price_cent <=> other.price_cent;
      }

      bool operator==(const LimitOrder &other) const {
        return price_cent == other.price_cent;
      }

      friend std::ostream &operator<<(std::ostream &os, const LimitOrder &order) {
        os << "LimitOrder { timestamp: " << order.timestamp
            << ", type: " << (order.type == Type::Add ? "Add" : "Reduce")
            << ", id: " << order.id;
        if (order.type == Type::Add) {
          os << ", side: " << (order.side == Side::Ask ? "Ask" : "Bid")
              << ", price_cent: " << order.price_cent << ", size: " << order.size
              << " }";
        } else {
          os << ", size: " << order.size << " }";
        }

        return os;
      }
    };
  } // namespace Order
} // namespace OrderBookProgrammingProblem
#endif // ORDER_H
