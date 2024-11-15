#pragma once
#include <memory>

namespace GuiBridge {
class Graph;
class EdgeToPloyLine {
public:
    void process(std::shared_ptr<Graph> &graph);
};
}  // namespace GuiBridge