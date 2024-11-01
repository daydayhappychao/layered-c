#ifndef PORT_TYPE_HPP
#define PORT_TYPE_HPP
namespace GuiBridge {

enum class PortType {
    /** input port type. */
    INPUT,
    /** output port type. */
    OUTPUT,

};

PortType opposedPortType(PortType type);

}  // namespace GuiBridge
#endif  // PORT_TYPE_HPP