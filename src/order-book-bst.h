#ifndef ORDER_BOOK_BST_H
#define ORDER_BOOK_BST_H

#include "utils.h"
#include "order-book-interface.h"
#include "order.h"
#include "bst-impl.h"
#include "price-level-doubly-linked-list.h"
#include "price-level-array.h"
#include "price-level-dict.h"
#include "price-level-interface.h"

#include <memory>
#include <unordered_map>
#include <vector>

namespace OrderBookProgrammingProblem {
  using PriceLevelImpl = PriceLevelArray;
  using BST = BinarySearchTree<PriceLevelImpl>;

  class OrderBookBst : public IOrderBook<OrderBookBst> {
    TreeNode<PriceLevelImpl> *ask_prices = nullptr;
    TreeNode<PriceLevelImpl> *bid_prices = nullptr;
    std::unordered_map<std::string, std::shared_ptr<Order::LimitOrder> > order_by_id;

    std::string get_string_repr_ask_side() {
      return "Ask:";
    }

    std::string get_string_repr_bid_side() {
      return "Bid:";
    }

  public:
    OrderBookBst() = default;

    std::optional<int> FUNC_ATTRIBUTE get_pricer_sell_cost_cent_impl(int target_size) const {
      int cost_cent = 0;
      auto prices = BST::inorder_traversal(bid_prices);
      for (auto &price_level: std::views::reverse(prices)) {
        auto level_price_cent = price_level.get_level_price();
        auto level_size = price_level.get_level_size();
        if (!level_price_cent.has_value() || !level_size.has_value())
          throw std::logic_error("!level_price_cent.has_value() || !level_size.has_value()");
        if (target_size > level_size.value()) {
          target_size -= level_size.value();
          cost_cent += level_price_cent.value() * level_size.value();
        } else {
          cost_cent += target_size * level_price_cent.value();
          target_size = 0;
          return cost_cent;
        }
      }
      return std::nullopt;
    }

    std::optional<int> FUNC_ATTRIBUTE get_pricer_buy_cost_cent_impl(int target_size) {
      int cost_cent = 0;
      auto prices = BST::inorder_traversal(ask_prices);
      for (auto &price_level: prices) {
        auto level_price_cent = price_level.get_level_price();
        auto level_size = price_level.get_level_size();
        if (!level_price_cent.has_value() || !level_size.has_value())
          throw std::logic_error("!level_price_cent.has_value() || !level_size.has_value()");
        if (target_size > level_size.value()) {
          target_size -= level_size.value();
          cost_cent += level_price_cent.value() * level_size.value();
        } else {
          cost_cent += target_size * level_price_cent.value();
          target_size = 0;
          return cost_cent;
        }
      }
      return std::nullopt;
    }

    void add_order_impl(const Order::LimitOrder &new_order) {
      const auto order_ptr = std::make_shared<Order::LimitOrder>(new_order);
      PriceLevelImpl temp_price_level;
      if (new_order.type == Order::Type::Add) {
        auto &ba_prices =
            new_order.side == Order::Side::Ask ? ask_prices : bid_prices;

        temp_price_level.add_order(order_ptr);
        const auto price_level = BST::search(ba_prices, temp_price_level);
        if (price_level != nullptr)
          price_level->val.add_order(order_ptr);
        else
          BST::insert(&ba_prices, temp_price_level);

        order_by_id[new_order.id] = order_ptr;
        return;
      }

      std::shared_ptr<Order::LimitOrder> existing_order = order_by_id[new_order.id];
      if (!existing_order) {
        throw std::invalid_argument("Order not found");
      }
      auto &ba_prices =
          existing_order->side == Order::Side::Ask ? ask_prices : bid_prices;
      temp_price_level.add_order(existing_order);
      const auto price_level = BST::search(ba_prices, temp_price_level);
      if (price_level == nullptr)
        throw std::logic_error("!price_level");
      price_level->val.update_order(order_ptr);
      if (price_level->val.get_level_size() <= 0) {
        BST::delete_node(&ba_prices, temp_price_level);
        order_by_id.erase(new_order.id);
      }
    }

    std::string to_string_impl() {
      return get_string_repr_ask_side() + get_string_repr_bid_side();
    }
  };
} // namespace OrderBookProgrammingProblem

#endif // ORDER_BOOK_BST_H
