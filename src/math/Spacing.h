#ifndef SPACING_H
#define SPACING_H

#include <memory>
#include <string>

namespace GuiBridge {

class Spacing {
public:
    double top = 0.0;
    double bottom = 0.0;
    double left = 0.0;
    double right = 0.0;

    Spacing();
    Spacing(double top, double right, double bottom, double left);

    void set(const Spacing &spacing);
    void set(double newTop, double newRight, double newBottom, double newLeft);

    double getTop() const;
    void setTop(double top);

    double getRight() const;
    void setRight(double right);

    double getBottom() const;
    void setBottom(double bottom);

    double getLeft() const;
    void setLeft(double left);

    void setLeftRight(double val);
    void setTopBottom(double val);

    double getHorizontal() const;
    double getVertical() const;

    bool equals(const Spacing &other) const;
    int hashCode() const;
    std::string toString() const;

    // 复制间距值的函数
    Spacing &copy(const Spacing &other);
    // 添加间距值的函数
    Spacing &add(const Spacing &other);

protected:
    // 确定给定字符是否为分隔符的辅助函数
    static bool isdelim(char c, const std::string &delims);
};
}  // namespace GuiBridge

#endif  // SPACING_H