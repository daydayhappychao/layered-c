#ifndef NODE_SIDE_HPP
#define NODE_SIDE_HPP
namespace GuiBridge {

enum class NodeSide {
    // 孤独的第一层，一般是最左侧的入口
    FIRST_SEPARATE,
    // 孤独的最后一层，一般是最右侧的入口
    LAST_SEPARATE,
    // 可共享的第一层，一般是与 FIRST_SEPARATE 连接的 NODE
    FIRST,
    // 可共享的最后一层，一般是与 LAST_SEPARATE 连接的 NODE
    LAST,
    NONE,
};
}
#endif  // NODE_SIDE_HPP