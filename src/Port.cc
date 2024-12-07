#include "Port.h"

#include <algorithm>
#include <memory>
#include <string>
#include <utility>

#include "nlohmann/json.hpp"
#include "opts/PortType.h"

namespace GuiBridge {

Port::Port(std::string name, int _id, PortType portType) : name(std::move(name)), portType(portType), _id(_id) {}

KVector Port::getAnchor() { return anchor; }

nlohmann::json Port::json() {
    nlohmann::json res;
    res["x"] = this->pos.x;
    res["y"] = this->pos.y;
    res["name"] = name;
    return res;
}
}  // namespace GuiBridge