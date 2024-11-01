#ifndef HIDDEN_NODE_CONNECTION_HPP
#define HIDDEN_NODE_CONNECTION_HPP
#include "../opts/NodeSide.h"

namespace GuiBridge {
class HiddenNodeConnections {
public:
    enum class Connection { NONE, FIRST_SEPARATE, LAST_SEPARATE, BOTH };

    // Combine method
    static Connection combine(Connection current, NodeSide layerConstraint) {
        switch (current) {
            case Connection::NONE:
                return layerConstraint == NodeSide::FIRST_SEPARATE ? Connection::FIRST_SEPARATE
                                                                   : Connection::LAST_SEPARATE;

            case Connection::FIRST_SEPARATE:
                return layerConstraint == NodeSide::FIRST_SEPARATE ? Connection::FIRST_SEPARATE : Connection::BOTH;

            case Connection::LAST_SEPARATE:
                return layerConstraint == NodeSide::FIRST_SEPARATE ? Connection::BOTH : Connection::LAST_SEPARATE;

            default:
                return Connection::BOTH;
        }
    }
};
}  // namespace GuiBridge

#endif