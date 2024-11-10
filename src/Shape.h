#ifndef SHAPE_HPP
#define SHAPE_HPP

#include <memory>
#include <string>
#include <vector>
#include "math/KVector.h"

namespace GuiBridge {
class Shape {
public:
    KVector &getPos();
    KVector &getSize();

protected:
    KVector pos = KVector();
    KVector size = KVector();
};
}  // namespace GuiBridge
#endif
