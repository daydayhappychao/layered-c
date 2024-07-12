
#include "Shape.h"
#include <memory>
#include <string>
#include <vector>
#include "math/KVector.h"

namespace GuiBridge {
KVector Shape::getPos() const { return this->pos; }
KVector Shape::getSize() const { return this->size; }
}  // namespace GuiBridge
