#pragma once
#include <future>
#include <optional>
#include "../GeneticOptimizer.hpp"
#include "../Graph/NodeGraph.hpp"
#include "../Math.hpp"
#include "PortGraogFitness.hpp"

namespace usagi {
struct PortGraphPopulationGenerator {
    node_graph::NodeGraph prototype;
    std::uniform_real_distribution<float> domain{0, 1};

    template <typename Optimizer>
    PortGraphIndividual operator()(Optimizer &o) {
        PortGraphIndividual individual;
        individual.genotype.resize(prototype.nodes.size() * 2);
        std::generate(individual.genotype.begin(), individual.genotype.end(),
                      // use ref for rng to prevent being copied
                      std::bind(domain, std::ref(o.rng)));
        individual.graph.base_graph = &prototype;
        individual.graph.node_positions = reinterpret_cast<Vector2f *>(individual.genotype.data());
        return individual;
    }
};
}  // namespace usagi