#ifndef ORDER_BOOK_ARRAY_H
#define ORDER_BOOK_ARRAY_H

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

namespace OrderBookProgrammingProblem {
  using PriceLevelImpl = PriceLevelArray;


  template<typename T>
  concept AllowedPriceLevelImpl =
      std::same_as<T, PriceLevelArray> || std::same_as<T, PriceLevelDict>;

  class OrderBookArray : public IOrderBook<OrderBookArray> {
    std::vector<PriceLevelImpl> ask_prices;
    std::vector<PriceLevelImpl> bid_prices;
    std::unordered_map<std::string, std::shared_ptr<Order::LimitOrder> >
    order_by_id;
    double bid_hit = 0, bid_miss = 0;
    double ask_hit = 0, ask_miss = 0;
    int lowest_bidding_cent = INT_MAX;
    int lowest_asking_cent = INT_MAX;

    template<AllowedView View1, AllowedView View2>
    static std::string get_repr_of_one_side(std::vector<PriceLevelImpl> &price_levels,
                                            View1 transform1, View2 transform2) {
      std::vector<std::string> levels; // = label + ":\n";
      int accu_volume = 0, accu_size = 0;

      size_t level = 0;

      for (PriceLevelImpl &price_level: transform1(price_levels)) {
        auto level_price_cent = price_level.get_level_price();
        if (level_price_cent.has_value()) {
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
        }
      }

      return (levels | transform2 | std::views::join_with(std::string("\n")) |
              std::ranges::to<std::string>()) +
             "\n";
    }

    std::string get_string_repr_ask_side() {
      return "Ask:\n" + get_repr_of_one_side<decltype(std::views::all),
               decltype(std::views::reverse)>(
               ask_prices, std::views::all, std::views::reverse);
    }

    std::string get_string_repr_bid_side() {
      return "Bid:\n" + get_repr_of_one_side<decltype(std::views::reverse),
               decltype(std::views::all)>(
               bid_prices, std::views::reverse, std::views::all);
    }

  public:
    OrderBookArray() = default;

    std::optional<int>
    FUNC_ATTRIBUTE get_pricer_sell_cost_cent_impl(int target_size) {
      int cost_cent = 0;
      if (bid_prices.empty())
        return std::nullopt;
      for (auto i = bid_prices.size() - 1; i >= lowest_bidding_cent; --i) {
        auto price_level = bid_prices[i];
        auto level_price_cent = price_level.get_level_price();
        if (!level_price_cent.has_value()) {
          if constexpr (!benchmark_performance) {
            ++bid_miss;
            std::cerr << "[bid_miss] lowest_bidding_cent: " << lowest_bidding_cent
                << ", i: " << i << ", hit: " << bid_hit
                << ", miss: " << bid_miss
                << ", hit rate: " << bid_hit / (bid_hit + bid_miss) << "\n";
          }
          continue;
        }
        auto level_size = price_level.get_level_size();
        if (!level_size.has_value()) {
          continue;
        }
        if constexpr (!benchmark_performance) {
          ++bid_hit;
          std::cerr << "[bid_hit] lowest_bidding_cent: " << lowest_bidding_cent
              << ", i: " << i << ", hit: " << bid_hit
              << ", miss: " << bid_miss
              << ", hit rate: " << bid_hit / (bid_hit + bid_miss) << "\n";
        }

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

    std::optional<int>
    FUNC_ATTRIBUTE get_pricer_buy_cost_cent_impl(int target_size) {
      /*
      std::cerr << "get_pricer_buy_cost_cent_impl()\n";
      return get_pricer_cost_cent(ask_prices, std::views::all, target_size);
      */
      int cost_cent = 0;
      // This loop is hugely inefficient, say there is only one bid price at 40
      // but size smaller than target_size, then it iterates from 4000 to 0 use
      // ssize_t to have -1 support
      for (ssize_t i = lowest_asking_cent; i < ask_prices.size(); ++i) {
        auto price_level = ask_prices[i];
        auto level_price_cent = price_level.get_level_price();
        if (!level_price_cent.has_value()) {
          ++ask_miss;
          if constexpr (!benchmark_performance)
            std::cerr << "[ask_miss] lowest_asking_cent: " << lowest_asking_cent
                << ", i: " << i
                << ", ask_prices.size(): " << ask_prices.size()
                << ", hit: " << ask_hit << ", miss: " << ask_miss
                << ", hit rate: " << ask_hit / (ask_hit + ask_miss) << "\n";

          continue;
        }
        auto level_size = price_level.get_level_size();
        if (!level_size.has_value()) {
          continue;
        }
        ++ask_hit;
        if constexpr (!benchmark_performance)
          std::cerr << "[ask_hit] lowest_asking_cent: " << lowest_asking_cent
              << ", i: " << i
              << ", ask_prices.size(): " << ask_prices.size()
              << ", hit: " << ask_hit << ", miss: " << ask_miss
              << ", hit rate: " << ask_hit / (ask_hit + ask_miss) << "\n";

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
      if (new_order.type == Order::Type::Add) {
        auto &prices =
            new_order.side == Order::Side::Ask ? ask_prices : bid_prices;

        if (prices.size() <= new_order.price_cent) {
          prices.resize(new_order.price_cent + 1);
        }

        if (new_order.side == Order::Side::Bid &&
            new_order.price_cent < lowest_bidding_cent) {
          lowest_bidding_cent = new_order.price_cent;
        }
        if (new_order.side == Order::Side::Ask &&
            new_order.price_cent < lowest_asking_cent) {
          lowest_asking_cent = new_order.price_cent;
        }

        prices[new_order.price_cent].add_order(order_ptr);
        order_by_id[new_order.id] = order_ptr;
        return;
      }

      const auto existing_order = order_by_id[new_order.id];
      if (!existing_order) {
        throw std::invalid_argument("Order not found");
      }

      auto update_order = [&](auto &prices) {
        auto new_price_level_size =
            prices[existing_order->price_cent].update_order(order_ptr);
        if (new_price_level_size <= 0) {
          order_by_id.erase(new_order.id);
          if (existing_order->side == Order::Side::Ask) {
            if (order_ptr->price_cent <= lowest_asking_cent) {
              for (int i = lowest_asking_cent; i < ask_prices.size(); ++i) {
                if (ask_prices[i].get_level_size_impl().has_value()) {
                  lowest_asking_cent = i;
                  break;
                }
              }
            }
            while (!ask_prices.empty()) {
              if (!ask_prices[ask_prices.size() - 1]
                .get_level_size_impl()
                .has_value())
                ask_prices.pop_back();
              else
                break;
            }
          } else {
            if (order_ptr->price_cent <= lowest_bidding_cent) {
              for (int i = lowest_bidding_cent; i < bid_prices.size(); ++i) {
                if (bid_prices[i].get_level_size_impl().has_value()) {
                  lowest_bidding_cent = i;
                  break;
                }
              }
            }
            while (!bid_prices.empty()) {
              if (!bid_prices[bid_prices.size() - 1]
                .get_level_size_impl()
                .has_value())
                bid_prices.pop_back();
              else
                break;
            }
          }
        }
        /*
        for (auto const &order : prices[existing_order->price_cent]) {
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
        }*/
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
