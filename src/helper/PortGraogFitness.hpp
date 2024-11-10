#ifndef PORT_GRAOGFITNESS_HPP
#define PORT_GRAOGFITNESS_HPP
#include <optional>
#include "../GeneticOptimizer.hpp"
#include "../Graph/NodeGraph.hpp"
#include "../Math.hpp"

namespace usagi {

std::optional<Vector2f> get_line_intersection(const Vector2f &p0, const Vector2f &p1, const Vector2f &p2,
                                              const Vector2f &p3, const Vector2f &ignore0, const Vector2f &ignore1);

struct PortGraphIndividual : genetic::Individual<std::vector<float>, float> {
    node_graph::NodeGraphInstance graph;

    float f_overlap = 0;
    float f_link_pos = 0;
    float f_link_angle = 0;
    float f_link_crossing = 0;
    float f_link_node_crossing = 0;
    int c_angle = 0;
    int c_invert_pos = 0;

    std::vector<Vector2f> crosses;

    static constexpr std::size_t BEZIER_SEGMENT_COUNT = 6;
    static constexpr std::size_t BEZIER_POINT_COUNT = BEZIER_SEGMENT_COUNT + 1;
    struct BezierInfo {
        std::array<Vector2f, BEZIER_POINT_COUNT> points;
        AlignedBox2f bbox;
        float factor_a = 0;
        float factor_b = 0;
    };
    std::vector<BezierInfo> bezier_curves;
};

struct PortGraphFitness {
    using FitnessT = float;

    bool heuristic = true;
    bool center_graph = false;
    int grid = 1;
    float p_max_angle = 60;
    float p_min_pos_x = 50;

    float node_overlap_penalty = -1000;
    float edge_crossing_penalty = -100;
    float edge_node_crossing_penalty = -100;

    std::size_t countEdgeCrossings(PortGraphIndividual &g, std::size_t link_idx);
    std::size_t countNodeEdgeCrossings(PortGraphIndividual &g, std::size_t link_idx, float control_factor_a,
                                       float control_factor_b, bool insert_crossings);
    FitnessT operator()(PortGraphIndividual &g);
};
}  // namespace usagi

#endif