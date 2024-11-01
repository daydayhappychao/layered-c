#include "PortType.h"

namespace GuiBridge {
PortType opposedPortType(PortType type) {
    switch (type) {
        case PortType::INPUT:
            return PortType::OUTPUT;
        case PortType::OUTPUT:
            return PortType::INPUT;
    }
}
}  // namespace GuiBridge