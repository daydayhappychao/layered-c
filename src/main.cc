#include <iostream>
#include <ostream>
#include "Edge.h"
#include "Graph.h"
#include "LayeredEnginee.h"
#include "Node.h"
#include "Port.h"
#include "opts/NodeSide.h"
namespace GuiBridge {
int main() {
    // 运行 ELK Layered 算法
    ELKLayered elkLayered("data/test1");
    elkLayered.layered();
    // elkLayered.printLayers();
    // elkLayered.printJson();
    std::cout << "1112" << std::endl;
    return 0;
}
}  // namespace GuiBridge

int main() { return GuiBridge::main(); }