#ifndef ORDER_H
#define ORDER_H

#include <iostream>
#include <string>
#include <format>

namespace OrderBookProgrammingProblem {
    namespace Order {
        enum struct Side : char { Bid = 'B', Ask = 'A' };

        enum struct Type : char { Add = 'A', Reduce = 'R' };

        struct LimitOrder {
            uint64_t timestamp = 0;
            Type type = Type::Add;
            std::string id;
            Side side = Side::Bid;
            float price = 0;
            int size = 0;

            // This is for testing data structures, dont remove it for now
            LimitOrder(const int price) : price(price) {
            }

            LimitOrder() = default;

            // Custom three-way comparison
            auto operator<=>(const LimitOrder &other) const {
                return price <=> other.price;
            }

            bool operator==(const LimitOrder &other) const {
                return price == other.price;
            }

            friend std::ostream &operator<<(std::ostream &os, const LimitOrder &order) {
                os << "LimitOrder { timestamp: " << order.timestamp
                        << ", type: " << static_cast<char>(order.type)
                        << ", id: " << order.id
                        << ", side: " << static_cast<char>(order.side)
                        << ", price: " << order.price
                        << ", size: " << order.size
                        << " }";
                return os;
            }
        };
    }
}
#endif // ORDER_H
