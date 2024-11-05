#ifndef BK_ALIGNED_LAYOUT_HPP
#define BK_ALIGNED_LAYOUT_HPP

#include <vector>

namespace GuiBridge {
class Graph;
class NeighborhoodInformation;
class Node;
class Port;

enum class VDirection { DOWN, UP };
enum class HDirection { RIGHT, LEFT };

class BKAlignedLayout {
public:
    BKAlignedLayout(std::shared_ptr<Graph> &layeredGraph, int nodeCount, VDirection vdir, HDirection hdir);
    void cleanup();
    double layoutSize();
    double calculateDelta(std::shared_ptr<Port> &src, std::shared_ptr<Port> &tgt);
    void shiftBlock(std::shared_ptr<Node> &rootNode, double delta);
    double checkSpaceAbove(std::shared_ptr<Node> &blockRoot, double delta, NeighborhoodInformation *ni);
    double checkSpaceBelow(std::shared_ptr<Node> &blockRoot, double delta, NeighborhoodInformation *ni);
    double getMinY(std::shared_ptr<Node> &n);
    double getMaxY(std::shared_ptr<Node> &n);
    std::string toString();

private:
    std::vector<std::shared_ptr<Node>> root;
    std::vector<double> blockSize;
    std::vector<std::shared_ptr<Node>> align;
    std::vector<double> innerShift;
    std::vector<std::shared_ptr<Node>> sink;
    std::vector<double> shift;
    std::vector<double> y;
    VDirection vdir;
    HDirection hdir;
    std::vector<bool> su;
    std::vector<bool> od;

    std::shared_ptr<Graph> layeredGraph;
    std::shared_ptr<Node> &getLowerNeighbor(std::shared_ptr<Node> &n, NeighborhoodInformation *ni);
    std::shared_ptr<Node> &getUpperNeighbor(std::shared_ptr<Node> &n, NeighborhoodInformation *ni);
};
}  // namespace GuiBridge

#endif