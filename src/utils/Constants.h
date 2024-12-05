#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

namespace GuiBridge {
const int THOROUGHNESS = 7;

/** Empirically determined threshold when removing subtrees pays off. */
const int REMOVE_SUBTREES_THRESH = 40;

const int LAYER_MARGIN = 50;
const float EDGE_EDGE_GAP = 10.0;
const float EDGE_NODE_GAP = 14.0;
const float NODE_NODE_GAP = 20.0;
}  // namespace GuiBridge
#endif