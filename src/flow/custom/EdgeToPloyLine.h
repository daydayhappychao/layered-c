#pragma once
#include <memory>
#include <vector>

namespace GuiBridge {
class Graph;
class KVector;
class LayerGapPosManage {
public:
    bool isAvaliable(int p);
    int applyNewXLeft();
    int applyNewXRight();
    static int applyMinCrossY(const KVector &startPoint,const KVector &endPoint, std::shared_ptr<Graph> &graph);

private:
    std::vector<int> leftPos;
    std::vector<int> rightPos;
};

class EdgeToPloyLine {
public:
    void process(std::shared_ptr<Graph> &graph);

private:
    // 记录 layer 之间已经被使用掉的 x 坐标
    std::vector<LayerGapPosManage> layerGapPos;
};
}  // namespace GuiBridge