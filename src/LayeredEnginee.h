#ifndef ELK_LAYERED_HPP
#define ELK_LAYERED_HPP

#include <filesystem>
#include <memory>
#include <queue>
#include <unordered_map>

#include "Graph.h"
namespace GuiBridge {

class ELKLayered {
public:
    explicit ELKLayered(std::shared_ptr<Graph> graph);
    explicit ELKLayered(const std::filesystem::path &path);
    void layered();
    void printLayers();
    void printJson();

private:
    std::shared_ptr<Graph> graph;
    std::unordered_map<std::shared_ptr<Node>, int> layer;
    void layout(const std::shared_ptr<Graph> &graph);
};
}  // namespace GuiBridge
#endif  // ELK_LAYERED_HPP
