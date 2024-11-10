#ifndef HYPER_EDGE_DUMMY_MERGER
#define HYPER_EDGE_DUMMY_MERGER

#include <vector>

namespace GuiBridge {
class Graph;
class Node;
class Port;

class HyperedgeDummyMerger {
public:
    void process(std::shared_ptr<Graph> &layeredGraph);

private:
    struct MergeState {
        bool allowMerge;
        bool sameSource;
        bool sameTarget;
        MergeState(bool allowMerge, bool sameSource, bool sameTarget);
    };
    MergeState checkMergeAllowed(std::shared_ptr<Node> &currNode, std::shared_ptr<Node> &lastNode);
    void mergeNodes(std::shared_ptr<Node> &mergeSource, std::shared_ptr<Node> &mergeTarget, bool keepSourcePort,
                    bool keepTargetPort);
    void identifyHyperedges(std::shared_ptr<Graph> &lGraph);
    void dfs(std::shared_ptr<Port> &p, int index);
};
}  // namespace GuiBridge

#endif