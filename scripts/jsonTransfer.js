const json = require("../data.json");
const fs = require("fs");

let res = "";

let protoJson = [];
let portJson = [];
let nodeJson = [];
let edgeJson = [];

let protoId = 0;
let portId = 0;
let nodeId = 0;
let edgeId = 0;

for (let i = 0; i < json.cells.length; i++) {
  const cell = json.cells[i];
  let inPinLen = cell.pins.filter((p) => p.direction == "input").length;
  let outPinLen = cell.pins.filter((p) => p.direction == "output").length;
  protoJson.push({
    id: protoId,
    displayName: cell.handle_name,
    width: 80,
    height: (Math.max(inPinLen, outPinLen) + 1) * 10,
  });

  for (let pin of cell.pins) {
    let name = pin.name;
    portJson.push({
      id: portId++,
      proto: protoId,
      name: name,
      type: pin.direction == "output" ? "out" : "in",
    });
  }

  nodeJson.push({
    id: nodeId++,
    proto: protoId,
    name: cell.name,
    pos: "default",
  });

  protoId++;
}

for (const port of json.ports) {
  protoJson.push({
    id: protoId,
    displayName: port.handle_name,
    width: 3,
    height: 3,
  });

  let name = port.name;
  portJson.push({
    id: portId++,
    proto: protoId,
    name: name,
    type: port.direction == "output" ? "in" : "out",
  });
  nodeJson.push({
    id: nodeId++,
    proto: protoId,
    name: port.name,
    pos: port.direction == "output" ? "east" : "west",
  });
  protoId++;
}

for (const net of json.nets) {
  const { handle_name, name, bits } = net;
  for (const bit of bits) {
    const inputPins = [];
    const outputPins = [];
    for (const cell of json.cells) {
      const p = cell.pins.filter((pin) => pin.bits.includes(bit));
      const node = nodeJson.find((node) => node.name === cell.name);
      const nodeId = node.id;
      const protoId = protoJson.find((proto) => proto.id == node.proto).id;
      for (const _p of p) {
        const portId = portJson.find(
          (port) => port.name == _p.name && port.proto === protoId
        )["id"];
        if (_p.direction == "output") {
          outputPins.push([nodeId, portId]);
        } else {
          inputPins.push([nodeId, portId]);
        }
      }
    }
    for (const port of json.ports) {
      if (port.bits.includes(bit)) {
        const node = nodeJson.find((node) => node.name === port.name);
        const nodeId = node.id;
        const protoId = protoJson.find((proto) => proto.id == node.proto).id;
        const portId = portJson.find(
          (pin) => pin.name == port.name && pin.proto === protoId
        )["id"];
        if (port.direction == "output") {
          inputPins.push([nodeId, portId]);
        } else {
          outputPins.push([nodeId, portId]);
        }
      }
    }
    if (inputPins.length === 1 && outputPins.length === 1) {
      edgeJson.push({
        outnode: outputPins[0][0],
        outpin: outputPins[0][1],
        innode: inputPins[0][0],
        inpin: inputPins[0][1],
        id: edgeId++,
        name,
      });
    } else if (inputPins.length === 1 && outputPins.length > 1) {
      for (const outputPin of outputPins) {
        edgeJson.push({
          outnode: outputPin[0],
          outpin: outputPin[1],
          innode: inputPins[0][0],
          inpin: inputPins[0][1],
          id: edgeId++,
          name,
        });
      }
    } else if (outputPins.length === 1 && inputPins.length > 1) {
      for (const inputPin of inputPins) {
        edgeJson.push({
          outnode: outputPins[0][0],
          outpin: outputPins[0][1],
          innode: inputPin[0],
          inpin: inputPin[1],
          id: edgeId++,
          name,
        });
      }
    } else {
      throw new Error("数据不合法");
    }
  }
}

edgeJson = edgeJson.reduce((pre, next) => {
  if (
    pre.find(
      (item) =>
        item.outnode == next.outnode &&
        item.outpin == next.outpin &&
        item.innode == next.innode &&
        item.inpin == next.inpin
    )
  ) {
    return pre;
  } else {
    return [...pre, next];
  }
}, []);

res += `#       id      displayname         width   height\n`;
protoJson.forEach((p) => {
  res += `proto    ${p.id}    ${p.displayName}    ${p.width}    ${p.height}\n`;
});

res += `\n`;
res += `#       id      proto   name \n`;

portJson.forEach((p) => {
  res += `port    ${p.id}    ${p.proto}    ${p.name}    ${p.type}\n`;
});

res += `\n`;
res += `#       id      proto   name   type\n`;

nodeJson.forEach((n) => {
  res += `node    ${n.id}    ${n.proto}    ${n.name}    ${n.pos}\n`;
});

res += `\n`;
res += `#       outnode outpin  innode  inpin  name\n`;

edgeJson.forEach((e) => {
  res += `edge    ${e.outnode}    ${e.outpin}    ${e.innode}    ${e.inpin} ${e.name}\n`;
});

fs.writeFileSync(`${__dirname}/../data/data`, res, "utf-8");
