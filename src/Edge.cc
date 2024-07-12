#include "Edge.h"

#include <memory>
#include <utility>

#include "./opts/PortType.h"
#include "./utils/GraphUtil.h"
#include "Port.h"
#include "math/KVectorChain.h"
#include "nlohmann/json.hpp"
#include "nlohmann/json_fwd.hpp"
namespace GuiBridge {

Edge::Edge(const std::shared_ptr<Port> &src, const std::shared_ptr<Port> &dst) {
    this->src = src;
    this->dst = dst;
}

std::shared_ptr<Port> Edge::getSrc() { return src.lock(); }

std::shared_ptr<Port> Edge::getDst() { return dst.lock(); }

void Edge::setSrc(const std::shared_ptr<Port> &nextSrc) { src = nextSrc; }
void Edge::setDst(const std::shared_ptr<Port> &nextDst) { dst = nextDst; }

void Edge::reverse(const std::shared_ptr<Graph> &layeredGraph, bool adaptPorts) {
    std::shared_ptr<Port> oldSource = getSrc();
    std::shared_ptr<Port> oldDst = getDst();
    setSrc(nullptr);
    setDst(nullptr);

    if (adaptPorts && oldDst->getInternalCollect()) {
        setSrc(provideCollectorPort(layeredGraph, oldDst->getNode(), OUTPUT));
    } else {
        setSrc(oldDst);
    }

    if (adaptPorts && oldSource->getInternalCollect()) {
        setDst(provideCollectorPort(layeredGraph, oldSource->getNode(), INPUT));
    } else {
        setDst(oldSource);
    }

    this->reversed = !this->reversed;
}

KVectorChain Edge::getBendPoints() { return bendPoints; }

nlohmann::json Edge::json() {
    nlohmann::json res;
    res["src"] = src.lock()->getName();
    res["dst"] = dst.lock()->getName();
    res["name"] = name;
    return res;
}
}  // namespace GuiBridge