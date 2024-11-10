#include <fstream>  // 包含文件流的头文件
#include <iostream>
#include <nlohmann/json.hpp>
#include <random>
#include "./Crossover.hpp"
#include "./GeneticOptimizer.hpp"
#include "./Graph/NodeGraph.hpp"
#include "./Mutation.hpp"
#include "./ParentSelection.hpp"
#include "./Replacement.hpp"
#include "./StopCondition.hpp"
#include "ParentSelection.hpp"
#include "helper/PortGraogFitness.hpp"
#include "helper/PortGraphPopulationGenerator.hpp"
#include "nlohmann/json_fwd.hpp"

using namespace usagi;

auto getBezierControlPoints(const Vector2f &p0, const Vector2f &p1, const float control_factor_a,
                            const float control_factor_b) {
    const Vector2f size = (p1 - p0).cwiseAbs();
    // const auto control_x = std::min(size.x(), 250.f);
    const auto control_x = size.x();

    return std::make_tuple(Vector2f(p0.x(), p0.y()), Vector2f(p0.x() + control_x * control_factor_a, p0.y()),
                           Vector2f(p1.x() - control_x * control_factor_b, p1.y()), Vector2f(p1.x(), p1.y()));
};
int main() {
    using Gene = float;
    using Genotype = std::vector<float>;
    using OptimizerT =
        genetic::GeneticOptimizer<Gene, PortGraphFitness, genetic::parent::TournamentParentSelection<5, 2>,
                                  genetic::crossover::WholeArithmeticRecombination,
                                  genetic::mutation::UniformRealMutation<Genotype>,
                                  genetic::replacement::RoundRobinTournamentReplacement<10, 2>,
                                  genetic::stop::SolutionConvergedStopCondition<float>, PortGraphPopulationGenerator,
                                  Genotype, PortGraphIndividual>;
    OptimizerT mOptimizer;

    mOptimizer.mutation.mutation_rate = 0.1;
    std::filesystem::path mGraphPath = "src/Data/default.ng";
    auto &graph = mOptimizer.generator.prototype = node_graph::NodeGraph::readFromFile(mGraphPath);

    const auto domain = std::uniform_real_distribution<float>{0.f, (graph.size.x())};

    mOptimizer.generator.domain = domain;

    mOptimizer.mutation.domain = domain;

    mOptimizer.initializePopulation(7);

    nlohmann::json res;

    auto &best = mOptimizer.best.top();
    auto &g = best->graph;
    auto &b = *g.base_graph;
    for (int i = 0; i < graph.nodes.size(); i++) {
        auto node = b.node(i);
        auto pos = g.mapNodeRegion(i);
        nlohmann::json nodeJson;
        nodeJson["name"] = node.name;
        nodeJson["displayName"] = node.prototype->name;
        nodeJson["x"] = pos.min().x();
        nodeJson["y"] = pos.min().y();
        nodeJson["width"] = pos.sizes().x();
        nodeJson["height"] = pos.sizes().y();
        for (auto &port : node.prototype->in_ports) {
            nlohmann::json portJson;
            auto item = node.prototype->portPosition(port, g.mapNodePosition(i));
            portJson["x"] = item.x();
            portJson["y"] = item.y();
            nodeJson["inPort"].emplace_back(portJson);
        }
        for (auto &port : node.prototype->out_ports) {
            nlohmann::json portJson;
            auto item = node.prototype->portPosition(port, g.mapNodePosition(i));
            portJson["x"] = item.x();
            portJson["y"] = item.y();
            nodeJson["outPort"].emplace_back(portJson);
        }
        res["node"].push_back(nodeJson);

        // for (auto &port : node.prototype->out_ports) {
        //     printf("%s,%f,%f\n", node.name.c_str(), node.prototype->size.x(), node.prototype->size.y());
        // }
    }

    for (std::size_t i = 0; i < b.links.size(); ++i) {
        auto &curve = best->bezier_curves[i];
        auto &points = curve.points;
        auto [p0, p1] = g.mapLinkEndPoints(i);
        auto [a, b, c, d] = getBezierControlPoints(p0, p1, curve.factor_a, curve.factor_b);

        nlohmann::json lineItem;

        for (std::size_t j = 0; j < curve.points.size() - 1; ++j) {
            nlohmann::json lineItemItem;
            lineItemItem["fromX"] = points[j].x();
            lineItemItem["fromY"] = points[j].y();
            lineItemItem["toX"] = points[j + 1].x();
            lineItemItem["toY"] = points[j + 1].y();
            lineItem.emplace_back(lineItemItem);
        }
        res["lines"].emplace_back(lineItem);
    }

    std::cout << res.dump(2) << std::endl;
    std::ofstream outfile("output.json");

    // 检查文件是否成功打开
    if (!outfile.is_open()) {
        std::cerr << "无法打开文件！" << std::endl;
        return 1;
    }

    // 将字符串写入文件
    outfile << res.dump(2);

    // 关闭文件
    outfile.close();

    std::cout << "写入文件成功！" << std::endl;
    return 0;
}