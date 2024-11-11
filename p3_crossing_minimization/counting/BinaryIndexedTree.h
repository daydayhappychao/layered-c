#ifndef BINARY_INDEXED_TREE_H
#define BINARY_INDEXED_TREE_H

namespace GuiBridge {
class BinaryIndexedTree {
public:
    explicit BinaryIndexedTree(int maxNum);
    void add(int index);
    int rank(int index) const;
    int size() const;
    void removeAll(int index);
    void clear();
    bool isEmpty() const;

private:
    int *binarySums;
    int *numsPerIndex;
    int size_;
    int maxNum_;
};
}  // namespace GuiBridge

#endif  // BINARY_INDEXED_TREE_H