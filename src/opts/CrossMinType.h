#ifndef CROSS_MIN_TYPE_HPP
#define CROSS_MIN_TYPE_HPP
namespace GuiBridge {

enum class CrossMinType {
    /** Use BarycenterHeuristic. */
    BARYCENTER,
    /** Use one-sided GreedySwitchHeuristic. */
    ONE_SIDED_GREEDY_SWITCH,
    /** Use two-sided GreedySwitchHeuristic. */
    TWO_SIDED_GREEDY_SWITCH
};

}  // namespace GuiBridge
#endif  // PORT_TYPE_HPP