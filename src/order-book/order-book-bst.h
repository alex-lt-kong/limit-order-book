#ifndef ORDER_BOOK_BST_H
#define ORDER_BOOK_BST_H

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

namespace OrderBookProgrammingProblem {
  using PriceLevelImpl = PriceLevelArray;
  using BST = BinarySearchTree<PriceLevelImpl>;

  class OrderBookBst final : public IOrderBook<OrderBookBst> {
    TreeNode<PriceLevelImpl> *ask_prices = nullptr;
    TreeNode<PriceLevelImpl> *bid_prices = nullptr;
    std::unordered_map<std::string, std::shared_ptr<Order::LimitOrder> >
    order_by_id;

    template<TraversalOrder TOrder, AllowedView View2>
    static std::string get_repr_of_one_side(TreeNode<PriceLevelImpl> *price_levels, View2 transform2) {
      std::vector<std::string> levels; // = label + ":\n";
      int accu_volume = 0, accu_size = 0;

      size_t level = 0;
      const BST::CallbackType on_new_price_level =
          [&](void *, PriceLevelArray &price_level) {
        auto level_price_cent = price_level.get_level_price();
        if (!level_price_cent.has_value()) {
          throw std::logic_error("!level_price_cent.has_value(), how come?");
        }
        levels.push_back(std::format("Level: {:>2}, Price: {:>5.02f}, ",
                                     ++level,
                                     level_price_cent.value() / 100.0));
        int level_size = price_level.get_level_size().value();

        std::string orders_detail = price_level.to_string();
        accu_size += level_size;
        accu_volume += level_size * level_price_cent.value();
        levels[levels.size() - 1] += std::format(
          "Size: {:>5}, AccuSize: {:>5}, AccuVolume: {:>8}, Orders: {}",
          level_size, accu_size, accu_volume, orders_detail);

        return true;
      };
      BST::inorder_traversal_cb<TOrder>(price_levels, on_new_price_level, nullptr);

      return (levels | transform2 | std::views::join_with(std::string("\n")) |
              std::ranges::to<std::string>()) +
             "\n";
    }

    std::string get_string_repr_ask_side() const {
      return "Ask:\n" + get_repr_of_one_side<TraversalOrder::RightRootLeft, decltype(std::views::reverse)>(
               ask_prices, std::views::reverse);
    }

    std::string get_string_repr_bid_side() const {
      return "Bid:\n" + get_repr_of_one_side<TraversalOrder::LeftRootRight, decltype(std::views::all)>(
               bid_prices, std::views::all);
    }

  public:
    OrderBookBst() = default;

    template<TraversalOrder TOrder>
    static std::optional<int>
    FUNC_ATTRIBUTE get_pricer_cost_cent_impl(TreeNode<PriceLevelImpl> *price_levels, int target_size) {
      int cost_cent = 0;
      const BST::CallbackType on_new_price_level =
          [&](void *, PriceLevelArray &price_level) {
        if (target_size == 0)
          throw std::logic_error(std::string("Unexpected target_size (0) for price_level: ") +
                                 std::to_string(price_level.get_level_price().value()));

        const auto level_price_cent = price_level.get_level_price();
        const auto level_size = price_level.get_level_size();
        if (!level_price_cent.has_value() || !level_size.has_value())
          throw std::logic_error(
            "!level_price_cent.has_value() || !level_size.has_value()");
        if (target_size > level_size.value()) {
          target_size -= level_size.value();
          cost_cent += level_price_cent.value() * level_size.value();
        } else {
          cost_cent += target_size * level_price_cent.value();
          target_size = 0;
          return false;
        }
        return true;
      };

      BST::inorder_traversal_cb<TOrder>(price_levels, on_new_price_level, nullptr);
      if (target_size == 0) {
        return cost_cent;
      }
      return std::nullopt;
    }

    std::optional<int>
    FUNC_ATTRIBUTE get_pricer_sell_cost_cent_impl(int target_size) const {
      return get_pricer_cost_cent_impl<TraversalOrder::LeftRootRight>(bid_prices, target_size);
    }

    std::optional<int>
    FUNC_ATTRIBUTE get_pricer_buy_cost_cent_impl(int target_size) const {
      return get_pricer_cost_cent_impl<TraversalOrder::RightRootLeft>(ask_prices, target_size);
    }

    void FUNC_ATTRIBUTE add_order_impl(const Order::LimitOrder &new_order) {
      const auto order_ptr = std::make_shared<Order::LimitOrder>(new_order);
      if (new_order.type == Order::Type::Add) {
        auto &ba_prices =
            new_order.side == Order::Side::Ask ? ask_prices : bid_prices;


        if (const auto price_level = BST::search(ba_prices, new_order.price_cent); price_level != nullptr)
          price_level->val.add_order(order_ptr);
        else {
          PriceLevelImpl temp_price_level;
          temp_price_level.add_order(order_ptr);
          BST::insert(&ba_prices, temp_price_level);
        }


        order_by_id[new_order.id] = order_ptr;
        return;
      }

      const std::shared_ptr<Order::LimitOrder> existing_order =
          order_by_id[new_order.id];
      if (!existing_order) {
        throw std::invalid_argument("Order not found");
      }

      auto &ba_prices =
          existing_order->side == Order::Side::Ask ? ask_prices : bid_prices;
      // temp_price_level.add_order(existing_order);
      auto price_level = BST::search(ba_prices, existing_order->price_cent);
      if (price_level == nullptr)
        throw std::logic_error("!price_level");
      price_level->val.update_order(order_ptr);
      if (price_level->val.get_level_size() <= 0) {
        //PriceLevelImpl temp_price_level;
        //temp_price_level.add_order(existing_order);
        BST::delete_node(&ba_prices, existing_order->price_cent);
        order_by_id.erase(new_order.id);
      }
    }

    std::string to_string_impl() const {
      return get_string_repr_ask_side() + get_string_repr_bid_side();
    }


    ~OrderBookBst() override {
      while (ask_prices != nullptr) {
        BST::delete_node(&ask_prices, ask_prices->val);
      }
      while (bid_prices != nullptr) {
        BST::delete_node(&bid_prices, bid_prices->val);
      }
    }
  };
} // namespace OrderBookProgrammingProblem

#endif // ORDER_BOOK_BST_H
