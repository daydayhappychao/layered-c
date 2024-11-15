#include "Edge.h"

#include <memory>
#include <stdexcept>
#include <utility>

#include "Node.h"
#include "Port.h"
#include "math/KVectorChain.h"
#include "nlohmann/json.hpp"
#include "nlohmann/json_fwd.hpp"
namespace GuiBridge {

Edge::Edge(std::shared_ptr<Node> &srcNode, std::shared_ptr<Port> &srcPort, std::shared_ptr<Node> &dstNode,
           std::shared_ptr<Port> &dstPort)
    : srcNode(srcNode),
      srcPort(srcPort),
      dstNode(dstNode),
      dstPort(dstPort){

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

EdgeTarget Edge::getOther(EdgeTarget &edgeTarget) { return getOther(edgeTarget.node); }

KVectorChain &Edge::getBendPoints() { return bendPoints; }

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

void Edge::setSrc(EdgeTarget &nextSrc) {
    auto thisPtr = shared_from_this();
    auto srcPortPtr = srcPort.lock();
    srcNode.lock()->removeEdge(srcPortPtr, thisPtr);

    this->srcNode = nextSrc.node;
    this->srcPort = nextSrc.port;

    auto nextSrcPortPtr = srcPort.lock();
    srcNode.lock()->addEdge(nextSrcPortPtr, thisPtr);
}

void Edge::setDst(EdgeTarget &nextDst) {
    auto thisPtr = shared_from_this();
    auto dstPortPtr = dstPort.lock();
    dstNode.lock()->removeEdge(dstPortPtr, thisPtr);

    this->dstNode = nextDst.node;
    this->dstPort = nextDst.port;

    auto nextDstPortPtr = dstPort.lock();
    dstNode.lock()->addEdge(nextDstPortPtr, thisPtr);
}

KVector Edge::getSrcPoint() {
    auto srcPtr = srcNode.lock();
    auto srcPortPtr = srcPort.lock();
    auto portPos = srcPtr->getPortPos(srcPortPtr);
    return portPos;
};

KVector Edge::getDstPoint() {
    auto dstPtr = dstNode.lock();
    auto dstPortPtr = dstPort.lock();
    auto dstPortPos = dstPtr->getPortPos(dstPortPtr);
    return dstPortPos;
};

nlohmann::json Edge::json() {
    nlohmann::json res;
    auto srcPtr = srcNode.lock();
    auto portPos = getSrcPoint();

    auto dstPtr = dstNode.lock();
    auto dstPortPos = getDstPoint();
    res["src"] = srcPtr->name;
    res["srcX"] = portPos.x;
    res["srcY"] = portPos.y;
    res["dst"] = dstNode.lock()->name;
    res["dstX"] = dstPortPos.x;
    res["dstY"] = dstPortPos.y;
    res["name"] = name;
    for (auto &point : getBendPoints()) {
        nlohmann::json item;
        item["x"] = point.x;
        item["y"] = point.y;
        res["bendPoints"].emplace_back(item);
    }
    return res;
}
}  // namespace GuiBridge