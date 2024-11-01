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

std::shared_ptr<Port> Edge::getOther(const std::shared_ptr<Node> &someNode) {
    if (src.lock()->getNode() == someNode) {
        return dst.lock();
    }
    if (dst.lock()->getNode() == someNode) {
        return src.lock();
    }
    return nullptr;
}

void Edge::setSrc(const std::shared_ptr<Port> &nextSrc) {
    auto ptr = shared_from_this();
    if (this->src.lock() != nullptr) {
        this->src.lock()->removeEdge(ptr);
    }
    if (nextSrc != nullptr) {
        nextSrc->addEdge(ptr);
    }
    src = nextSrc;
}
void Edge::setDst(const std::shared_ptr<Port> &nextDst) {
    auto ptr = shared_from_this();
    if (this->dst.lock() != nullptr) {
        this->dst.lock()->removeEdge(ptr);
    }
    if (nextDst != nullptr) {
        nextDst->addEdge(ptr);
    }
    dst = nextDst;
}
std::shared_ptr<Port> Edge::setOppositePort(OppositeType oppositeType) {
    this->oppositeType = oppositeType;
    switch (oppositeType) {
        case Src:
            this->oppositePort = this->src;
            this->setSrc(nullptr);
            break;
        case Dst:
            this->oppositePort = this->dst;
            this->setDst(nullptr);
            break;
        case None:
            this->revertOppositePort();
            break;
    }
    return this->oppositePort.lock();
};

void Edge::revertOppositePort() {
    switch (this->oppositeType) {
        case Src:
            this->setSrc(this->oppositePort.lock());
            break;
        case Dst:
            this->setDst(this->oppositePort.lock());
            break;
        case None:
            break;
    }
    this->oppositeType = None;
    std::shared_ptr<Port> sp = nullptr;  // 空的 shared_ptr
    this->oppositePort = sp;
}

void Edge::reverse(const std::shared_ptr<Graph> &layeredGraph, bool adaptPorts) {
    std::shared_ptr<Port> oldSource = getSrc();
    std::shared_ptr<Port> oldDst = getDst();
    setSrc(nullptr);
    setDst(nullptr);

    if (adaptPorts && oldDst->getInternalCollect()) {
        setSrc(provideCollectorPort(layeredGraph, oldDst->getNode(), PortType::OUTPUT));
    } else {
        setSrc(oldDst);
    }

    if (adaptPorts && oldSource->getInternalCollect()) {
        setDst(provideCollectorPort(layeredGraph, oldSource->getNode(), PortType::INPUT));
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