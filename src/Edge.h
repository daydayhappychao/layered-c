#ifndef EDGE_HPP
#define EDGE_HPP

#include <memory>
#include <optional>
#include <string>

#include "Port.h"
#include "math/KVectorChain.h"
#include "nlohmann/json_fwd.hpp"
namespace GuiBridge {

class Graph;

class Edge : public std::enable_shared_from_this<Edge> {
public:
    Edge(const std::shared_ptr<Port> &src, const std::shared_ptr<Port> &dst);

    std::shared_ptr<Port> getSrc();
    void setSrc(const std::shared_ptr<Port> &nextSrc);
    std::shared_ptr<Port> getDst();
    void setDst(const std::shared_ptr<Port> &nextDst);
    KVectorChain getBendPoints();

    void reverse(const std::shared_ptr<Graph> &layeredGraph, bool adaptPorts);

    nlohmann::json json();

private:
    std::weak_ptr<Port> src;
    std::weak_ptr<Port> dst;
    std::string name;
    KVectorChain bendPoints;
    bool reversed = false;
};
}  // namespace GuiBridge
#endif  // EDGE_HPP
