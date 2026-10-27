#include "parser.hpp"
#include <sstream>
#include <algorithm>

namespace OrderEngine {

std::string OrderParser::extractValue(const std::string& json, const std::string& key) {
    std::string search_key = "\"" + key + "\":";
    size_t pos = json.find(search_key);
    if (pos == std::string::npos) return "";
    
    pos = json.find_first_not_of(" \t", pos + search_key.length());
    if (pos == std::string::npos) return "";
    
    size_t end_pos;
    if (json[pos] == '"') {
        pos++;
        end_pos = json.find('"', pos);
    } else {
        end_pos = json.find_first_of(",}", pos);
    }
    
    if (end_pos == std::string::npos) return "";
    
    return json.substr(pos, end_pos - pos);
}

std::optional<std::unique_ptr<Order>> OrderParser::parseOrder(const std::string& json) {
    std::string side_str = extractValue(json, "side");
    std::string price_str = extractValue(json, "price");
    std::string quantity_str = extractValue(json, "quantity");
    
    if (side_str.empty() || price_str.empty() || quantity_str.empty()) {
        return std::nullopt;
    }
    
    OrderSide side;
    if (side_str == "buy") {
        side = OrderSide::BUY;
    } else if (side_str == "sell") {
        side = OrderSide::SELL;
    } else {
        return std::nullopt;
    }
    
    try {
        double price = std::stod(price_str);
        uint32_t quantity = std::stoul(quantity_str);
        
        auto order = std::make_unique<Order>(0, side, price, quantity);
        return order;
    } catch (...) {
        return std::nullopt;
    }
}

} // namespace OrderEngine
