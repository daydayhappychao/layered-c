#include <iostream>
#include <ostream>
#include "LayeredEnginee.h"
namespace GuiBridge {
int main() {
    // 运行 ELK Layered 算法
    ELKLayered elkLayered("data/data");
    elkLayered.layered();
    // elkLayered.printLayers();
    // elkLayered.printJson();
    std::cout << "1112" << std::endl;
    return 0;
}
}  // namespace GuiBridge

int main() { return GuiBridge::main(); }