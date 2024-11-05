#include "Spacing.h"

namespace GuiBridge {

// 默认构造函数
Spacing::Spacing() {}

// 带参数的构造函数
Spacing::Spacing(double top, double right, double bottom, double left) { set(top, right, bottom, left); }

// 设置所有间距为给定Spacing对象的间距
void Spacing::set(const Spacing &spacing) { set(spacing.top, spacing.right, spacing.bottom, spacing.left); }

// 同时设置四个间距
void Spacing::set(double newTop, double newRight, double newBottom, double newLeft) {
    this->top = newTop;
    this->right = newRight;
    this->bottom = newBottom;
    this->left = newLeft;
}

// 获取顶部间距
double Spacing::getTop() const { return top; }

// 设置顶部间距
void Spacing::setTop(double top) { this->top = top; }

// 获取右侧间距
double Spacing::getRight() const { return right; }

// 设置右侧间距
void Spacing::setRight(double right) { this->right = right; }

// 获取底部间距
double Spacing::getBottom() const { return bottom; }

// 设置底部间距
void Spacing::setBottom(double bottom) { this->bottom = bottom; }

// 获取左侧间距
double Spacing::getLeft() const { return left; }

// 设置左侧间距
void Spacing::setLeft(double left) { this->left = left; }

// 设置左右间距为相同值
void Spacing::setLeftRight(double val) {
    this->left = val;
    this->right = val;
}

// 设置上下间距为相同值
void Spacing::setTopBottom(double val) {
    this->top = val;
    this->bottom = val;
}

// 获取水平方向的总间距（左 + 右）
double Spacing::getHorizontal() const { return getLeft() + getRight(); }

// 获取垂直方向的总间距（上 + 下）
double Spacing::getVertical() const { return getTop() + getBottom(); }

// 判断两个Spacing对象是否相等
bool Spacing::equals(const Spacing &other) const {
    return this->top == other.top && this->bottom == other.bottom && this->left == other.left &&
           this->right == other.right;
}

// 计算哈希码，这里的计算方式类似Java版本，但在C++中哈希码的使用场景和Java有所不同
int Spacing::hashCode() const {
    // 这里忽略了Java版本中关于MagicNumber的检查风格相关内容，因为C++没有严格类似的概念
    double leftValue = static_cast<double>(left);
    double bottomValue = static_cast<double>(bottom);
    double rightValue = static_cast<double>(right);
    double topValue = static_cast<double>(top);

    int code1 = static_cast<int>(leftValue) << 16;
    code1 |= static_cast<int>(bottomValue) & 0xffff;

    int code2 = static_cast<int>(rightValue) << 16;
    code2 |= static_cast<int>(topValue) & 0xffff;

    return code1 ^ code2;
}

// 将Spacing对象转换为字符串表示形式
std::string Spacing::toString() const {
    return "[top=" + std::to_string(top) + ",left=" + std::to_string(left) + ",bottom=" + std::to_string(bottom) +
           ",right=" + std::to_string(right) + "]";
}

// 复制间距值的函数实现
Spacing &Spacing::copy(const Spacing &other) {
    this->left = other.left;
    this->right = other.right;
    this->top = other.top;
    this->bottom = other.bottom;
    return *this;
}

// 添加间距值的函数实现
Spacing &Spacing::add(const Spacing &other) {
    this->left += other.left;
    this->right += other.right;
    this->top += other.top;
    this->bottom += other.bottom;
    return *this;
}

// 确定给定字符是否为分隔符的辅助函数实现
bool Spacing::isdelim(char c, const std::string &delims) {
    for (size_t i = 0; i < delims.length(); i++) {
        if (c == delims[i]) {
            return true;
        }
    }
    return false;
}
}  // namespace GuiBridge
