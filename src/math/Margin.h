#ifndef MARGIN_HPP
#define MARGIN_HPP

#include "Spacing.h"
namespace GuiBridge {
class Margin : public Spacing {
public:
    Margin() = default;
    explicit Margin(double any);
    Margin(double leftRight, double topBottom);
    Margin(double top, double right, double bottom, double left);
    Margin(const Margin &other);
};
}  // namespace GuiBridge

#endif