#include "Edge.h"
#include "Graph.h"
#include "LayeredEnginee.h"
#include "Node.h"
#include "Port.h"
namespace GuiBridge {
int main() {
    auto graph = std::make_shared<Graph>();

    // 创建节点
    auto node1 = std::make_shared<Node>("a");
    auto node2 = std::make_shared<Node>("b");
    auto node3 = std::make_shared<Node>("c");
    auto node4 = std::make_shared<Node>("d");
    auto node5 = std::make_shared<Node>("e");

    // 添加节点到图
    graph->addNode(node1);
    graph->addNode(node2);
    graph->addNode(node3);
    graph->addNode(node4);
    graph->addNode(node5);

    // 创建端口
    auto port1_1 = std::make_shared<Port>("a1");
    auto port1_2 = std::make_shared<Port>("a2");
    auto port2_1 = std::make_shared<Port>("b1");
    auto port2_2 = std::make_shared<Port>("b2");
    auto port3_1 = std::make_shared<Port>("c1");
    auto port3_2 = std::make_shared<Port>("c2");
    auto port4_1 = std::make_shared<Port>("d1");
    auto port4_2 = std::make_shared<Port>("d2");
    auto port5_1 = std::make_shared<Port>("e1");
    auto port5_2 = std::make_shared<Port>("e2");

    // 添加端口到节点
    node1->addInputPort(port1_1);
    node1->addOutputPort(port1_2);
    node2->addInputPort(port2_1);
    node2->addOutputPort(port2_2);
    node3->addInputPort(port3_1);
    node3->addOutputPort(port3_2);
    node4->addInputPort(port4_1);
    node4->addOutputPort(port4_2);
    node5->addInputPort(port5_1);
    node5->addOutputPort(port5_2);

    // 创建边
    auto edge1 = std::make_shared<Edge>(port1_2, port2_1);
    auto edge2 = std::make_shared<Edge>(port2_2, port3_1);
    auto edge3 = std::make_shared<Edge>(port3_2, port4_1);
    auto edge4 = std::make_shared<Edge>(port4_2, port5_1);
    auto edge5 = std::make_shared<Edge>(port5_2, port3_1);
    auto edge6 = std::make_shared<Edge>(port3_2, port1_1);
    edge1->init();
    edge2->init();
    edge3->init();
    edge4->init();
    edge5->init();
    edge6->init();

    // 添加边到图
    graph->addEdge(edge1);
    graph->addEdge(edge2);
    graph->addEdge(edge3);
    graph->addEdge(edge4);
    graph->addEdge(edge5);
    graph->addEdge(edge6);

    // 运行 ELK Layered 算法
    ELKLayered elkLayered(graph);
    elkLayered.layered();
    // elkLayered.printLayers();
    elkLayered.printJson();
    return 0;
}
}  // namespace GuiBridge

int main() { return GuiBridge::main(); }