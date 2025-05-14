#ifndef ORDER_BOOK_STD_MAP_H
#define ORDER_BOOK_STD_MAP_H

#include "../bst-impl.h"
#include "../order.h"
#include "../price-level/price-level-array.h"
#include "../price-level/price-level-dict.h"
#include "../price-level/price-level-doubly-linked-list.h"
#include "../price-level/price-level-interface.h"
#include "../utils.h"
#include "order-book-interface.h"

#include <memory>
#include <unordered_map>
#include <vector>
#include <format>
#include <map>

namespace OrderBookProgrammingProblem {
  using PriceLevelImpl = PriceLevelArray;
  using BST = BinarySearchTree<PriceLevelImpl>;

  class OrderBookStdMap final : public IOrderBook<OrderBookStdMap> {
    std::map<int, PriceLevelImpl> ask_prices;
    std::map<int, PriceLevelImpl> bid_prices;
    std::unordered_map<std::string, std::shared_ptr<Order::LimitOrder> >
    order_by_id;

    std::string get_string_repr_ask_side() const {
      return "Ask:\n<NotImplemented>\n"
          /* + get_repr_of_one_side<TraversalOrder::RightRootLeft, decltype(std::views::reverse)>(
                         ask_prices, std::views::reverse)*/;
    }

    std::string get_string_repr_bid_side() const {
      return "Bid:\nNotImplemented\n"
          /* + get_repr_of_one_side<TraversalOrder::LeftRootRight, decltype(std::views::all)>(
                         bid_prices, std::views::all)*/;
    }

  public:
    OrderBookStdMap() = default;

    template<AllowedView View1>
    static std::optional<int> FUNC_ATTRIBUTE get_pricer_cost_cent_impl(std::map<int, PriceLevelImpl> &price_levels,
                                                                       View1 transform, int target_size) {
      int cost_cent = 0;

      for (auto &price_level: transform(price_levels)) {
        const auto level_price_cent = price_level.first;
        const auto level_size = price_level.second.get_level_size();
        if (!level_size.has_value())
          throw std::logic_error("!level_size.has_value()");
        if (target_size > level_size.value()) {
          target_size -= level_size.value();
          cost_cent += level_price_cent * level_size.value();
        } else {
          cost_cent += target_size * level_price_cent;
          target_size = 0;
          return cost_cent;
        }
      }
      return std::nullopt;
    }

    std::optional<int> FUNC_ATTRIBUTE get_pricer_sell_cost_cent_impl(const int target_size) {
      return get_pricer_cost_cent_impl<decltype(std::views::reverse)>(bid_prices, std::views::reverse, target_size);
    }

    std::optional<int> FUNC_ATTRIBUTE get_pricer_buy_cost_cent_impl(const int target_size) {
      return get_pricer_cost_cent_impl<decltype(std::views::all)>(ask_prices, std::views::all, target_size);
    }

    void FUNC_ATTRIBUTE add_order_impl(const Order::LimitOrder &new_order) {
      const auto order_ptr = std::make_shared<Order::LimitOrder>(new_order);
      if (new_order.type == Order::Type::Add) {
        auto &ba_prices = new_order.side == Order::Side::Ask ? ask_prices : bid_prices;
        if (!ba_prices.contains(order_ptr->price_cent)) {
          ba_prices.insert({order_ptr->price_cent, PriceLevelImpl()});
        }
        ba_prices[order_ptr->price_cent].add_order(order_ptr);
        order_by_id[new_order.id] = order_ptr;
        return;
      }

      const std::shared_ptr<Order::LimitOrder> existing_order = order_by_id[new_order.id];
      if (!existing_order) {
        throw std::invalid_argument("!existing_order");
      }

      auto &ba_prices = existing_order->side == Order::Side::Ask ? ask_prices : bid_prices;
      if (!ba_prices.contains(existing_order->price_cent))
        throw std::logic_error("!price_level");
      auto price_level = ba_prices[existing_order->price_cent];
      price_level.update_order(order_ptr);
      if (price_level.get_level_size() <= 0) {
        // BST::delete_node(&ba_prices, existing_order->price_cent);
        ba_prices.erase(existing_order->price_cent);
        order_by_id.erase(new_order.id);
      }
    }

    std::string to_string_impl() const {
      return get_string_repr_ask_side() + get_string_repr_bid_side();
    }


    ~OrderBookStdMap() override = default;
  };
} // namespace OrderBookProgrammingProblem

#endif // ORDER_BOOK_STD_MAP_H
