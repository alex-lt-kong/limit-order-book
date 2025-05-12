#ifndef ORDER_BOOK_ARRAY_H
#define ORDER_BOOK_ARRAY_H

#include "order.h"
#include "order-book-interface.h"

#include <memory>
#include <unordered_map>
#include <vector>
#include <unordered_set>

namespace OrderBookProgrammingProblem {
    class OrderBookArray : public IOrderBook<OrderBookArray> {
        std::vector<std::unordered_map<std::string, std::shared_ptr<Order::LimitOrder> > > ask_prices;
        std::vector<std::unordered_map<std::string, std::shared_ptr<Order::LimitOrder> > > bid_prices;
        std::unordered_map<std::string, std::shared_ptr<Order::LimitOrder> >
        order_by_id;

        template<typename View>
        // noinline is for benchmark purpose
        std::optional<int> __attribute__((noinline)) get_pricer_cost_cent(
            const std::vector<std::unordered_map<std::string, std::shared_ptr<Order::LimitOrder> > > &prices,
            const View &transform, int target_size) {
            int cost_cent = 0;
            for (const auto &price_level: transform(prices)
            ) {
                //for (const auto &order: std::views::reverse(price_level)) {
                for (const auto &order: price_level) {
                    if (target_size > order.second->size) {
                        target_size -= order.second->size;
                        cost_cent += order.second->size * order.second->price_cent;
                    } else {
                        cost_cent += target_size * order.second->price_cent;
                        target_size = 0;
                        return cost_cent;
                    }
                }
            }
            return std::nullopt;
        }

        template<typename View1, typename View2>
        std::string get_repr_of_one_side(
            std::vector<std::unordered_map<std::string, std::shared_ptr<Order::LimitOrder> > > &price_levels,
            View1 transform1,
            View2 transform2) {
            std::vector<std::string> levels; // = label + ":\n";
            int accu_volume = 0, accu_size = 0;

            size_t level = 0;

            for (const auto &price_level: transform1(price_levels)) {
                if (!price_level.empty()) {
                    levels.push_back(std::format("Level: {:>2}, Price: {:>5.02f}, ",
                                                 ++level, price_level.begin()->second->price_cent / 100.0));
                    int level_size = 0;

                    std::string orders_detail = "{ ";
                    for (const auto &order: price_level) {
                        level_size += order.second->size;
                        orders_detail += std::format("{{ id: {:>3}, size: {:>3} }}, ",
                                                     order.second->id, order.second->size);
                    }
                    orders_detail.pop_back();
                    orders_detail.pop_back();
                    orders_detail += " }";

                    accu_size += level_size;
                    accu_volume += level_size * price_level.begin()->second->price_cent;
                    levels[levels.size() - 1] += std::format(
                        "Size: {:>5}, AccuSize: {:>5}, AccuVolume: {:>8}, Orders: {}",
                        level_size, accu_size, accu_volume, orders_detail);
                }
            }

            return (levels | transform2 | std::views::join_with(std::string("\n"))
                    | std::ranges::to<std::string>()) + "\n";
        }

        std::string get_string_repr_ask_side() {
            return "Ask:\n" + get_repr_of_one_side<decltype(std::views::all), decltype(std::views::reverse)>(
                       ask_prices, std::views::all, std::views::reverse);
        }

        std::string get_string_repr_bid_side() {
            return "Bid:\n" + get_repr_of_one_side<decltype(std::views::reverse), decltype(std::views::all)>(
                       bid_prices, std::views::reverse, std::views::all);
        }

    public:
        std::optional<int> get_pricer_sell_cost_cent_impl(
            const int target_size) {
            // Think about it, for get_pricer_cost_cent, if I want to complete its type,
            // like get_pricer_cost_cent<T>(prices, std::views::reverse, target_size);
            // what would T be?
            return get_pricer_cost_cent(bid_prices, std::views::reverse, target_size);
        }

        std::optional<int> get_pricer_buy_cost_cent_impl(
            const int target_size) {
            return get_pricer_cost_cent<decltype(std::views::all)>(
                ask_prices, std::views::all, target_size);
        }

        void add_order_impl(const Order::LimitOrder &new_order) {
            const auto order_ptr = std::make_shared<Order::LimitOrder>(new_order);
            if (new_order.type == Order::Type::Add) {
                auto &prices = new_order.side == Order::Side::Ask ? ask_prices : bid_prices;

                if (prices.size() <= new_order.price_cent) {
                    prices.resize(new_order.price_cent + 1);
                }

                prices[new_order.price_cent][order_ptr->id] = order_ptr;
                order_by_id[new_order.id] = order_ptr;
                return;
            }

            const auto existing_order = order_by_id[new_order.id];
            if (!existing_order) {
                throw std::invalid_argument("Order not found");
            }

            auto update_order = [&](auto &prices) {
                for (auto const &order: prices[existing_order->price_cent]) {
                    if (order.second->id == new_order.id) {
                        order.second->size -= new_order.size;
                        if (order.second->size < 0) {
                            throw std::invalid_argument("new_order size is negative");
                        }
                        if (order.second->size == 0) {
                            prices[existing_order->price_cent].erase(order.first);
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

        std::string to_string_impl() {
            return get_string_repr_ask_side() + get_string_repr_bid_side();
        }
    };
} // namespace OrderBookProgrammingProblem

#endif // ORDER_BOOK_ARRAY_H
