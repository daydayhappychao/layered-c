
#include "./Initializable.h"
#include <vector>
#include "../../../Node.h"
#include "../../../Port.h"

namespace GuiBridge {
void Initializable::init(const std::vector<Initializable *> &initializables,
                         std::vector<std::vector<std::shared_ptr<Node>>> &order) {
    for (int l = 0; l < order.size(); l++) {
        for (Initializable *i : initializables) {
            i->initAtLayerLevel(l, order);
        }
        for (int n = 0; order[l][n] != nullptr; n++) {
            for (Initializable *i : initializables) {
                i->initAtNodeLevel(l, n, order);
            }
            auto ports = order[l][n]->getAllPorts();
            for (int p = 0; p < ports.size(); p++) {
                for (Initializable *i : initializables) {
                    i->initAtPortLevel(l, n, p, order);
                }
                auto &port = ports[p];
                int e = 0;
                for (auto &edge : port->getEdges()) {
                    for (Initializable *i : initializables) {
                        i->initAtEdgeLevel(l, n, p, e++, edge, order);
                    }
                }
            }
        }
    }
    for (Initializable *i : initializables) {
        i->initAfterTraversal();
    }
}
}  // namespace GuiBridge