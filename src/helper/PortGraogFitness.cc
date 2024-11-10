#include "PortGraogFitness.hpp"
#include <numeric>
#include "../Math.hpp"

namespace usagi {
std::optional<Vector2f> get_line_intersection(const Vector2f &p0, const Vector2f &p1, const Vector2f &p2,
                                               const Vector2f &p3, const Vector2f &ignore0, const Vector2f &ignore1) {
    const Vector2f s1 = p1 - p0;
    const Vector2f s2 = p3 - p2;

    float s, t;
    s = (-s1.y() * (p0.x() - p2.x()) + s1.x() * (p0.y() - p2.y())) / (-s2.x() * s1.y() + s1.x() * s2.y());
    t = (s2.x() * (p0.y() - p2.y()) - s2.y() * (p0.x() - p2.x())) / (-s2.x() * s1.y() + s1.x() * s2.y());

    if (s >= 0 && s <= 1 && t >= 0 && t <= 1) {
        const Vector2f x = p0 + t * s1;
        if (x == ignore0) return {};
        if (x == ignore1) return {};
        // sometimes we have crossing exactly at segment crossing,
        // so cannot do this.
        // if(x == p0 || x == p1 || x == p2 || x == p3) return false;
        return {x};
    }

    return {};
}
std::size_t PortGraphFitness::countEdgeCrossings(PortGraphIndividual &g, std::size_t i) {
    std::size_t cross = 0;

    auto *base_graph = g.graph.base_graph;
    const auto link_count = base_graph->links.size();
    auto &curve = g.bezier_curves[i];

    // estimate bezier intersections
    // for each other curves
    for (std::size_t j = i + 1; j < link_count; ++j) {
        // for each our line segments
        for (std::size_t ii = 0; ii < curve.points.size() - 1; ++ii) {
            // test against their line segments
            for (std::size_t jj = 0; jj < g.bezier_curves[j].points.size() - 1; ++jj) {
                if (!curve.bbox.intersects(g.bezier_curves[j].bbox)) continue;
                auto x = get_line_intersection(curve.points[ii], curve.points[ii + 1], g.bezier_curves[j].points[jj],
                                                g.bezier_curves[j].points[jj + 1],
                                                // don't count lines starting from the same port
                                                curve.points.front(),
                                                // don't count lines ending at the same port
                                                curve.points.back());
                if (x.has_value()) {
                    g.crosses.push_back(x.value());
                    ++cross;
                }
                // g.f_link_crossing += edge_crossing_penalty;
            }
        }
    }
    return cross;
}
auto getBezierControlPoints(const Vector2f &p0, const Vector2f &p1, const Vector2f &offset,
                            const float control_factor_a, const float control_factor_b) {
    const Vector2f size = (p1 - p0).cwiseAbs();
    // const auto control_x = std::min(size.x(), 250.f);
    const auto control_x = size.x();

    return std::make_tuple(Vector2f(p0.x() + offset.x(), p0.y() + offset.y()),
                           Vector2f(p0.x() + offset.x() + control_x * control_factor_a, p0.y() + offset.y()),
                           Vector2f(p1.x() + offset.x() - control_x * control_factor_b, p1.y() + offset.y()),
                           Vector2f(p1.x() + offset.x(), p1.y() + offset.y()));
}

template <std::size_t I>
void PathBezierCurveTo(std::array<Vector2f, I> &points, const Vector2f &p1, const Vector2f &p2, const Vector2f &p3,
                       const Vector2f &p4) {
    points[0] = p1;
    float t_step = 1.0f / (float)(I - 1);
    for (int i_step = 1; i_step <= I - 1; i_step++) {
        float t = t_step * i_step;
        float u = 1.0f - t;
        float w1 = u * u * u;
        float w2 = 3 * u * u * t;
        float w3 = 3 * u * t * t;
        float w4 = t * t * t;
        points[i_step] = Vector2f(w1 * p1.x() + w2 * p2.x() + w3 * p3.x() + w4 * p4.x(),
                                  w1 * p1.y() + w2 * p2.y() + w3 * p3.y() + w4 * p4.y());
    }
}
std::size_t PortGraphFitness::countNodeEdgeCrossings(PortGraphIndividual &g, const std::size_t i,
                                                     const float control_factor_a, const float control_factor_b,
                                                     const bool insert_crossings) {
    auto *base_graph = g.graph.base_graph;
    const auto node_count = base_graph->nodes.size();

    auto &curve = g.bezier_curves[i];
    curve.factor_a = control_factor_a;
    curve.factor_b = control_factor_b;
    // build bezier curves and bounding box
    auto [p0, p1] = g.graph.mapLinkEndPoints(i);
    auto [a, b, c, d] = getBezierControlPoints(p0, p1, Vector2f::Zero(), curve.factor_a, curve.factor_b);
    // PathBezierToCasteljau(bezier_points[i], a, b, c, d);
    PathBezierCurveTo(curve.points, a, b, c, d);
    curve.bbox = AlignedBox2f();
    for (auto &&p : curve.points) {
        curve.bbox.extend(p);
    }

    std::size_t cross = 0;

    // estimate bezier and node intersections

    // for each node box
    for (std::size_t j = 0; j < node_count; ++j) {
        auto r = g.graph.mapNodeRegion(j);
        // the curve cannot intersect with this node
        if (!curve.bbox.intersects(r)) continue;
        // test our line segments with each of the node box edges
        for (std::size_t ii = 0; ii < curve.points.size() - 1; ++ii) {
            const std::pair<AlignedBox2f::CornerType, AlignedBox2f::CornerType> box_edges[] = {
                {AlignedBox2f::TopLeft, AlignedBox2f::TopRight},
                {AlignedBox2f::BottomLeft, AlignedBox2f::BottomRight},
                {AlignedBox2f::TopLeft, AlignedBox2f::BottomLeft},
                {AlignedBox2f::TopRight, AlignedBox2f::BottomRight},
            };
            for (auto &&e : box_edges) {
                auto x = get_line_intersection(curve.points[ii], curve.points[ii + 1], r.corner(e.first),
                                                r.corner(e.second),
                                                // don't count line beginning and ending as crossings
                                                curve.points.front(), curve.points.back());
                if (x.has_value()) {
                    // tentatively test to find the best routing
                    if (insert_crossings) g.crosses.push_back(x.value());
                    ++cross;
                }
                // g.f_link_node_crossing += edge_node_crossing_penalty;
            }
        }
    }
    return cross;
}

PortGraphFitness::FitnessT PortGraphFitness::operator()(PortGraphIndividual &g) {
    float fit = 0;
    auto *base_graph = g.graph.base_graph;

    // centers graph
    if (center_graph) {
        const auto center = g.graph.base_graph->size.x() * 0.5f;
        const auto sum = std::accumulate(g.genotype.begin(), g.genotype.end(), 0.f);
        const auto mean = sum / g.genotype.size();
        std::transform(g.genotype.begin(), g.genotype.end(), g.genotype.begin(),
                       [=](float v) { return v - mean + center; });
    }

    if (grid != 1) {
        std::transform(g.genotype.begin(), g.genotype.end(), g.genotype.begin(),
                       [this](float v) { return std::floor(v / grid) * grid; });
    }

    g.f_overlap = 0;
    g.f_link_pos = 0;
    g.f_link_angle = 0;
    g.f_link_crossing = 0;
    g.f_link_node_crossing = 0;
    g.c_angle = 0;
    g.c_invert_pos = 0;
    const auto node_count = base_graph->nodes.size();
    const auto link_count = base_graph->links.size();
    g.bezier_curves.resize(link_count);
    g.crosses.clear();
    // calculate overlapped area
    for (std::size_t i = 0; i < node_count; ++i) {
        auto r0 = g.graph.mapNodeRegion(i);
        for (auto j = i + 1; j < node_count; ++j) {
            auto r1 = g.graph.mapNodeRegion(j);
            const auto overlapped = r0.intersection(r1);
            if (!overlapped.isEmpty()) g.f_overlap += node_overlap_penalty;
        }
    }
    // measure angles and edge directions
    for (std::size_t i = 0; i < link_count; ++i) {
        auto [p0, p1] = g.graph.mapLinkEndPoints(i);
        Vector2f edge_diff = p1 - p0;
        Vector2f normalized_edge = edge_diff.normalized();
        // normalized edge direction using dot product. prefer edge towards
        // right.
        const auto angle = std::acos(normalized_edge.dot(Vector2f::UnitX()));
        // output port is to the left of input port
        g.f_link_pos += std::min(edge_diff.x(), p_min_pos_x);
        if (edge_diff.x() < p_min_pos_x) ++g.c_invert_pos;
        // prefer smaller angle
        const auto deg_angle = radiansToDegrees(angle);
        g.f_link_angle -= std::max(p_max_angle, deg_angle);
        if (deg_angle > p_max_angle) ++g.c_angle;
    }
    // calculate link position
    // const auto link_count = base_graph->links.size();
    // for(std::size_t i = 0; i < link_count; ++i)
    // {
    //     auto [pos0, pos1] = g.graph.mapLinkEndPoints(i);
    //     g.f_link_pos -= (pos0 - pos1).norm();
    // }
    // calculate link angle

    for (std::size_t m = 0; m < link_count; ++m) {
        if (heuristic) {
            std::array<float, 4> ctrl_factor = {0.8f, 0.6f, 0.4f, 0.2f};
            constexpr auto size = ctrl_factor.size();
            struct setting {
                float ca, cb;
                std::size_t en_cross;

                bool operator<(setting &rhs) const {
                    // try to reduce edge-node crossings
                    return en_cross < rhs.en_cross;
                }
            };
            std::array<setting, size * size> cross_count;
            // fill combinations
            {
                std::size_t k = 0;
                for (std::size_t i = 0; i < ctrl_factor.size(); ++i) {
                    for (std::size_t j = 0; j < ctrl_factor.size(); ++j) {
                        cross_count[k++] = {ctrl_factor[i], ctrl_factor[j], 0};
                    }
                }
            }
            std::stable_sort(cross_count.begin(), cross_count.end(),
                             [](auto &a, auto &b) { return std::abs(a.ca - a.cb) < std::abs(b.ca - b.cb); });
            for (auto &&s : cross_count) {
                s.en_cross = countNodeEdgeCrossings(g, m, s.ca, s.cb, false);
            }
            const auto min = std::min_element(cross_count.begin(), cross_count.end());
            // generating bezier curve segments here
            const auto x = countNodeEdgeCrossings(g, m, min->ca, min->cb, true);
            g.f_link_node_crossing += x * edge_node_crossing_penalty;
        } else {
            g.f_link_node_crossing += edge_node_crossing_penalty * countNodeEdgeCrossings(g, m, 0.8f, 0.8f, true);
        }
    }
    for (std::size_t m = 0; m < link_count; ++m) {
        g.f_link_crossing += edge_crossing_penalty * countEdgeCrossings(g, m);
    }

    fit = g.f_overlap + g.f_link_pos + g.f_link_angle + g.f_link_crossing + g.f_link_node_crossing;
    /*for(auto &&l : base_graph->links)
    {
        auto [n0, p0, n1, p1] = base_graph->mapLink(l);

        auto r0 =

        // f -= std::abs((pos0 - pos1).norm() - 100.f);
        // // f -= (p1 - p0).norm();
        // f -= std::abs((pos1 - pos0).dot(Vector2f::UnitY()));
        // prefer

        Vector2f edge_diff = pos1 - pos0;
        Vector2f normalized_edge = edge_diff.normalized();
        // normalized edge direction using dot product. prefer edge towards
        // right.
        const auto edge_direction = normalized_edge.dot(Vector2f::UnitX());
        // prefer given edge length
        const auto edge_length = -std::pow(edge_diff.norm() - 300.f, 2.f) + 1;
        fit += w_dir * edge_direction
            + w_length * edge_length;
    }*/
    return fit;
}
}  // namespace usagi