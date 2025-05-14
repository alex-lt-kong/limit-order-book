#include "../order.h"
#include "../utils.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace Problem = OrderBookProgrammingProblem;
namespace Order = Problem::Order;

TEST(OrderTest, TestStringRepr1) {
  Order::LimitOrder lo;
  lo.timestamp = 1234567890;
  lo.type = Order::Type::Add;
  lo.side = Order::Side::Ask;
  lo.price_cent = 314;
  lo.size = 123;
  lo.id = "0xdeadbeef";

  const std::stringstream ss;

  // turn on std::cout redirection
  std::streambuf *old = std::cout.rdbuf(ss.rdbuf());
  std::cout << lo;
  // turn off std::cout redirection
  std::cout.rdbuf(old);

  // Verify output matches expected string
  EXPECT_EQ(ss.str(), "LimitOrder { timestamp: 1234567890, type: Add, id: "
            "0xdeadbeef, side: Ask, price_cent: 314, size: 123 }");
}

TEST(OrderTest, TestStringRepr2) {
  Order::LimitOrder lo;
  lo.timestamp = 2147483647;
  lo.type = Order::Type::Reduce;
  lo.side = Order::Side::Bid;
  lo.price_cent = 1414;
  lo.size = 9527;
  lo.id = "Helloworld";

  const std::stringstream ss;

  // turn on std::cout redirection
  std::streambuf *old = std::cout.rdbuf(ss.rdbuf());
  std::cout << lo;
  // turn off std::cout redirection
  std::cout.rdbuf(old);

  // Verify output matches expected string
  EXPECT_EQ(ss.str(), "LimitOrder { timestamp: 2147483647, type: Reduce, id: "
            "Helloworld, size: 9527 }");
}

TEST(OrderTest, AtMostNDecimalPlaces) {
  Problem::Utils utils;
  EXPECT_TRUE(utils.at_most_n_decimal_places("3.14", 2));
  EXPECT_FALSE(utils.at_most_n_decimal_places("3.14", 1));
  EXPECT_FALSE(utils.at_most_n_decimal_places("3.1415", 2));
  EXPECT_TRUE(utils.at_most_n_decimal_places("1.4142135623", 65535));
  EXPECT_FALSE(utils.at_most_n_decimal_places("1.4142135623", 0));
  EXPECT_FALSE(utils.at_most_n_decimal_places("2.71", 0));
  EXPECT_TRUE(utils.at_most_n_decimal_places("9527", 0));
  EXPECT_TRUE(utils.at_most_n_decimal_places("0", 0));
  EXPECT_TRUE(utils.at_most_n_decimal_places("-2147483647", 1024));
  EXPECT_TRUE(utils.at_most_n_decimal_places("-3.2100", 4));
}

TEST(OrderTest, TestInputLines) {
  Problem::Utils utils;
  std::vector<std::string> lines = {
    "28800538 A b S 44.26 100", "28800562 A c B 44.10 100",
    "28800744 R b 100", "28800758 A d B 44.18 157",
    "28800773 A e S 44.38 100", "28800796 R d 157",
    "28800812 A f B 44.18 157", "28800974 A g S 44.27 100"
  };
  int idx = 0;
  auto lo = utils.parse_limit_order(lines[idx++]);
  EXPECT_EQ(lo.id, "b");
  EXPECT_EQ(lo.type, Order::Type::Add);

  lo = utils.parse_limit_order(lines[idx++]);
  EXPECT_EQ(lo.timestamp, 28800562);
  EXPECT_EQ(lo.price_cent, 4410);

  lo = utils.parse_limit_order(lines[idx++]);
  EXPECT_EQ(lo.timestamp, 28800744);
  EXPECT_EQ(lo.type, Order::Type::Reduce);
  EXPECT_EQ(lo.id, "b");
  EXPECT_EQ(lo.price_cent, 0);

  lo = utils.parse_limit_order(lines[idx++]);
  EXPECT_EQ(lo.timestamp, 28800758);
  EXPECT_EQ(lo.type, Order::Type::Add);
  EXPECT_EQ(lo.id, "d");
  EXPECT_EQ(lo.price_cent, 4418);
  EXPECT_EQ(lo.size, 157);

  lo = utils.parse_limit_order(lines[idx++]);
  EXPECT_EQ(lo.timestamp, 28800773);
  EXPECT_EQ(lo.type, Order::Type::Add);
  EXPECT_EQ(lo.id, "e");
  EXPECT_EQ(lo.side, Order::Side::Ask);
  EXPECT_EQ(lo.price_cent, 4438);
  EXPECT_EQ(lo.size, 100);

  lo = utils.parse_limit_order(lines[idx++]);
  EXPECT_EQ(lo.timestamp, 28800796);
  EXPECT_EQ(lo.type, Order::Type::Reduce);
  EXPECT_EQ(lo.id, "d");
  EXPECT_EQ(lo.size, 157);

  lo = utils.parse_limit_order(lines[idx++]);
  EXPECT_EQ(lo.timestamp, 28800812);
  EXPECT_EQ(lo.type, Order::Type::Add);
  EXPECT_EQ(lo.id, "f");
  EXPECT_EQ(lo.side, Order::Side::Bid);
  EXPECT_EQ(lo.price_cent, 4418);
  EXPECT_EQ(lo.size, 157);

  lo = utils.parse_limit_order(lines[idx++]);
  EXPECT_EQ(lo.timestamp, 28800974);
  EXPECT_EQ(lo.type, Order::Type::Add);
  EXPECT_EQ(lo.id, "g");
  EXPECT_EQ(lo.side, Order::Side::Ask);
  EXPECT_EQ(lo.price_cent, 4427);
  EXPECT_EQ(lo.size, 100);
}
