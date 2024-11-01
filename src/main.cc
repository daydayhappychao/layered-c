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
    auto graph = std::make_shared<Graph>();

    // 创建节点

    auto port1 = std::make_shared<Node>("port1", NodeSide::FIRST_SEPARATE);
    auto port2 = std::make_shared<Node>("port2", NodeSide::FIRST_SEPARATE);
    auto node1 = std::make_shared<Node>("node1");
    auto node2 = std::make_shared<Node>("node2");
    auto node3 = std::make_shared<Node>("node3");
    auto node4 = std::make_shared<Node>("node4");
    auto node5 = std::make_shared<Node>("node5");
    auto port3 = std::make_shared<Node>("port3", NodeSide::LAST_SEPARATE);

    // 添加节点到图

    graph->addNode(port1);
    graph->addNode(port2);
    graph->addNode(port3);
    graph->addNode(node1);
    graph->addNode(node2);
    graph->addNode(node3);
    graph->addNode(node4);
    graph->addNode(node5);

    // 创建端口
    auto port1_1 = std::make_shared<Port>("a1");
    auto port1_2 = std::make_shared<Port>("a2");
    auto port1_3 = std::make_shared<Port>("a3");
    auto port2_1 = std::make_shared<Port>("b1");
    auto port2_2 = std::make_shared<Port>("b2");
    auto port2_3 = std::make_shared<Port>("b3");
    auto port3_1 = std::make_shared<Port>("c1");
    auto port3_2 = std::make_shared<Port>("c2");
    auto port4_1 = std::make_shared<Port>("d1");
    auto port4_2 = std::make_shared<Port>("d2");
    auto port4_3 = std::make_shared<Port>("d3");
    auto port5_1 = std::make_shared<Port>("e1");
    auto port5_2 = std::make_shared<Port>("e2");
    auto port5_3 = std::make_shared<Port>("e3");
    auto port5_4 = std::make_shared<Port>("e4");
    auto port5_5 = std::make_shared<Port>("e5");

    auto fix_port_1_1 = std::make_shared<Port>("g1");
    auto fix_port_2_1 = std::make_shared<Port>("h1");
    auto fix_port_3_1 = std::make_shared<Port>("i1");

    // 添加端口到节点
    node1->addInputPort(port1_1);
    node1->addInputPort(port1_2);
    node1->addOutputPort(port1_3);

    node2->addInputPort(port2_1);
    node2->addOutputPort(port2_2);
    node2->addOutputPort(port2_3);

    node3->addInputPort(port3_1);
    node3->addOutputPort(port3_2);

    node4->addInputPort(port4_1);
    node4->addInputPort(port4_2);
    node4->addOutputPort(port4_3);

    node5->addInputPort(port5_1);
    node5->addInputPort(port5_2);
    node5->addInputPort(port5_3);
    node5->addOutputPort(port5_4);
    node5->addOutputPort(port5_5);

    port1->addOutputPort(fix_port_1_1);
    port2->addOutputPort(fix_port_2_1);
    port3->addInputPort(fix_port_3_1);

    // 创建边
    auto edge1 = std::make_shared<Edge>(fix_port_1_1, port1_1);
    auto edge2 = std::make_shared<Edge>(fix_port_2_1, port1_2);
    auto edge3 = std::make_shared<Edge>(fix_port_2_1, port2_1);
    auto edge4 = std::make_shared<Edge>(port1_3, port3_1);
    auto edge5 = std::make_shared<Edge>(port2_2, port4_1);
    auto edge6 = std::make_shared<Edge>(port2_3, port5_3);
    auto edge7 = std::make_shared<Edge>(port3_2, port5_1);
    auto edge8 = std::make_shared<Edge>(port4_3, port5_2);
    auto edge9 = std::make_shared<Edge>(port5_4, fix_port_3_1);
    auto edge10 = std::make_shared<Edge>(port5_5, port4_2);

    // 添加边到图
    graph->addEdge(edge1);
    graph->addEdge(edge2);
    graph->addEdge(edge3);
    graph->addEdge(edge4);
    graph->addEdge(edge5);
    graph->addEdge(edge6);
    graph->addEdge(edge7);
    graph->addEdge(edge8);
    graph->addEdge(edge9);
    graph->addEdge(edge10);

    // 运行 ELK Layered 算法
    ELKLayered elkLayered(graph);
    elkLayered.layered();
    // elkLayered.printLayers();
    // elkLayered.printJson();
    std::cout << "1112" << std::endl;
    return 0;
}
}  // namespace GuiBridge

int main() { return GuiBridge::main(); }