
#include "Shape.h"
#include <memory>
#include <string>
#include <vector>
#include "math/KVector.h"

namespace GuiBridge {
KVector& Shape::getPos() { return this->pos; }
KVector& Shape::getSize() { return this->size; }
}  // namespace GuiBridge
