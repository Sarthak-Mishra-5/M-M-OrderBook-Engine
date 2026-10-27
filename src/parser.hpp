#pragma once

#include <string>
#include <optional>
#include "types.hpp"

namespace OrderEngine {

class OrderParser {
public:
    std::optional<std::unique_ptr<Order>> parseOrder(const std::string& json);
    
private:
    std::string extractValue(const std::string& json, const std::string& key);
};

} // namespace OrderEngine
