#include "Margin.h"
#include "Spacing.h"  // 假设 Spacing 的头文件是 Spacing.h

namespace GuiBridge {

Margin::Margin(double any) : Spacing(any, any, any, any) {
    // 这里可能需要根据 Spacing 的构造函数具体实现调整
}

Margin::Margin(double leftRight, double topBottom) : Spacing(topBottom, leftRight, topBottom, leftRight) {
    // 同样可能需要根据 Spacing 的构造函数具体实现调整
}

Margin::Margin(double top, double right, double bottom, double left) : Spacing(top, right, bottom, left) {
    // 根据 Spacing 的构造函数具体实现调整
}

Margin::Margin(const Margin &other) : Spacing(other.top, other.right, other.bottom, other.left) {
    // 根据 Spacing 的构造函数具体实现调整
}

}  // namespace GuiBridge
