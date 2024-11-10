#include "Edge.h"

#include <memory>
#include <stdexcept>
#include <utility>

#include "./opts/PortType.h"
#include "./utils/GraphUtil.h"
#include "Node.h"
#include "Port.h"
#include "math/KVectorChain.h"
#include "nlohmann/json.hpp"
#include "nlohmann/json_fwd.hpp"
namespace GuiBridge {

Edge::Edge(std::shared_ptr<Node> &srcNode, std::shared_ptr<Port> &src, std::shared_ptr<Node> &dstNode,
           std::shared_ptr<Port> &dst){

};

EdgeTarget Edge::getSrc() {
    auto nodePtr = srcNode.lock();
    auto portPtr = srcPort.lock();
    EdgeTarget edgeTarget(nodePtr, portPtr);
    return edgeTarget;
}
EdgeTarget Edge::getDst() {
    auto nodePtr = dstNode.lock();
    auto portPtr = dstPort.lock();
    EdgeTarget edgeTarget(nodePtr, portPtr);
    return edgeTarget;
}

EdgeTarget Edge::getOther(const std::shared_ptr<Node> &someNode) {
    if (srcNode.lock() == someNode) {
        EdgeTarget edgeTarget(dstNode, dstPort);
        return edgeTarget;
    }
    if (dstNode.lock() == someNode) {
        EdgeTarget edgeTarget(srcNode, srcPort);
        return edgeTarget;
    }
    throw std::runtime_error("edge 与 node 不存在关联");
}

EdgeTarget Edge::getOther(EdgeTarget &edgeTarget) { getOther(edgeTarget.node); }

// void Edge::setSrc(EdgeTarget &nextSrc) {
//     auto ptr = shared_from_this();
//     if (this->src.lock() != nullptr) {
//         this->src.lock()->removeEdge(ptr);
//     }
//     if (nextSrc != nullptr) {
//         nextSrc->addEdge(ptr);
//     }
//     src = nextSrc;
// }
// void Edge::setDst(const std::shared_ptr<Port> &nextDst) {
//     auto ptr = shared_from_this();
//     if (this->dst.lock() != nullptr) {
//         this->dst.lock()->removeEdge(ptr);
//     }
//     if (nextDst != nullptr) {
//         nextDst->addEdge(ptr);
//     }
//     dst = nextDst;
// }
// std::shared_ptr<Port> Edge::setOppositePort(OppositeType oppositeType) {
//     this->oppositeType = oppositeType;
//     switch (oppositeType) {
//         case Src:
//             this->oppositePort = this->src;
//             this->setSrc(nullptr);
//             break;
//         case Dst:
//             this->oppositePort = this->dst;
//             this->setDst(nullptr);
//             break;
//         case None:
//             this->revertOppositePort();
//             break;
//     }
//     return this->oppositePort.lock();
// };

// void Edge::revertOppositePort() {
//     switch (this->oppositeType) {
//         case Src:
//             this->setSrc(this->oppositePort.lock());
//             break;
//         case Dst:
//             this->setDst(this->oppositePort.lock());
//             break;
//         case None:
//             break;
//     }
//     this->oppositeType = None;
//     std::shared_ptr<Port> sp = nullptr;  // 空的 shared_ptr
//     this->oppositePort = sp;
// }

// void Edge::reverse(const std::shared_ptr<Graph> &layeredGraph, bool adaptPorts) {
//     if (!this->reversed) {
//         std::shared_ptr<Port> oldSource = getSrc();
//         std::shared_ptr<Port> oldDst = getDst();
//         setSrc(nullptr);
//         setDst(nullptr);

//         if (adaptPorts && oldDst->getInternalCollect()) {
//             setSrc(provideCollectorPort(layeredGraph, oldDst->getNode(), PortType::OUTPUT));
//         } else {
//             setSrc(oldDst);
//         }

//         if (adaptPorts && oldSource->getInternalCollect()) {
//             setDst(provideCollectorPort(layeredGraph, oldSource->getNode(), PortType::INPUT));
//         } else {
//             setDst(oldSource);
//         }
//     } else {
//         setSrc(originSrc.lock());
//         setDst(originDst.lock());
//     }

//     this->reversed = !this->reversed;
// }

KVectorChain Edge::getBendPoints() { return bendPoints; }

bool Edge::isInLayerEdge() { return srcNode.lock()->getLayer() == dstNode.lock()->getLayer(); }

void Edge::hidden() {
    if (isHidden) {
        return;
    }
    isHidden = true;
    auto thisPtr = shared_from_this();
    auto srcPortPtr = srcPort.lock();
    srcNode.lock()->removeEdge(srcPortPtr, thisPtr);
    auto dstPortPtr = dstPort.lock();
    dstNode.lock()->removeEdge(dstPortPtr, thisPtr);
}

void Edge::show() {
    if (!isHidden) {
        return;
    }
    isHidden = false;
    auto edge = shared_from_this();
    auto portPtr = getSrc().port;
    getSrc().node->addEdge(portPtr, edge);
    auto dstPortPtr = getDst().port;
    getDst().node->addEdge(dstPortPtr, edge);
}

nlohmann::json Edge::json() {
    nlohmann::json res;
    res["src"] = srcNode.lock()->name;
    res["dst"] = dstNode.lock()->name;
    res["name"] = name;
    return res;
}
}  // namespace GuiBridge