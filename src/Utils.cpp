#include "Utils.h"


namespace OrderBookProgrammingProblem {
    void Utils::split_string(std::string_view str, char delimiter, std::vector<std::string_view> &result) {
        result.clear();
        size_t start = 0, end;

        while ((end = str.find(delimiter, start)) != std::string_view::npos) {
            result.push_back(str.substr(start, end - start));
            start = end + 1;
        }
        result.push_back(str.substr(start)); // Add last part
    }


    Order::LimitOrder Utils::parse_limit_order(const std::string &) {
        const std::string str = "28800538 A b S 44.26 100";
        std::vector<std::string_view> parts;
        split_string(str, ' ', parts);
        if (parts.size() != 6)
            throw std::invalid_argument("Unexpected parts.size() = " + std::to_string(parts.size()));
        Order::LimitOrder lo;

        auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), lo.timestamp);
        if (ec != std::errc()) {
            throw std::invalid_argument("Conversion failed");
        }
        lo.type = parts[1] == "A" ? Order::Type::Add : Order::Type::Bid;
        lo.id = parts[2];
        lo.side = parts[3] == "S" ? Order::Side::Ask : Order::Side::Bid;
        return Order::LimitOrder(10);
    };
} // OrderBookProgrammingProblem
