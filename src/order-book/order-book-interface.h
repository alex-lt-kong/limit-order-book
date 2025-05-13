#ifndef ORDER_BOOK_INTERFACE_H
#define ORDER_BOOK_INTERFACE_H

#include "../order.h"

#include <memory>
#include <ranges>


namespace OrderBookProgrammingProblem {
    template<typename T>
    class IOrderBook {
        static void add_order_impl(const Order::LimitOrder &new_order) {
            throw std::logic_error("Not implemented");
        }

        static std::optional<int> get_pricer_sell_cost_cent_impl(const int) {
            throw std::logic_error("Not implemented");
        }

        static std::optional<int> get_pricer_buy_cost_cent_impl(const int) {
            throw std::logic_error("Not implemented");
        }


        static std::string to_string_impl() {
            throw std::logic_error("Not implemented");
        }

    public:
        std::optional<int> get_pricer_sell_cost_cent(const int target_size) {
            return static_cast<T *>(this)->get_pricer_sell_cost_cent_impl(target_size);
        }

        std::optional<int> get_pricer_buy_cost_cent(const int target_size) {
            return static_cast<T *>(this)->get_pricer_buy_cost_cent_impl(target_size);
        }


        void add_order(const Order::LimitOrder &new_order) {
            return static_cast<T *>(this)->add_order_impl(new_order);
        }

        std::string to_string() {
            return static_cast<T *>(this)->to_string_impl();
        }

        virtual ~IOrderBook() = default;
    };
} // namespace OrderBookProgrammingProblem

#endif // ORDER_BOOK_INTERFACE_H
