#include "BinaryIndexedTree.h"
#include <cstring>
namespace GuiBridge {

BinaryIndexedTree::BinaryIndexedTree(int maxNum) : maxNum_(maxNum), size_(0) {
    binarySums = new int[maxNum + 1]();
    numsPerIndex = new int[maxNum]();
}

void BinaryIndexedTree::add(int index) {
    size_++;
    numsPerIndex[index]++;
    int i = index + 1;
    while (i < maxNum_ + 1) {
        binarySums[i]++;
        i += i & -i;
    }
}

int BinaryIndexedTree::rank(int index) const {
    int i = index;
    int sum = 0;
    while (i > 0) {
        sum += binarySums[i];
        i -= i & -i;
    }
    return sum;
}

int BinaryIndexedTree::size() const { return size_; }

void BinaryIndexedTree::removeAll(int index) {
    int numEntries = numsPerIndex[index];
    if (numEntries == 0) {
        return;
    }
    numsPerIndex[index] = 0;
    size_ -= numEntries;
    int i = index + 1;
    while (i < maxNum_ + 1) {
        binarySums[i] -= numEntries;
        i += i & -i;
    }
}

void BinaryIndexedTree::clear() {
    std::memset(binarySums, 0, (maxNum_ + 1) * sizeof(int));
    std::memset(numsPerIndex, 0, maxNum_ * sizeof(int));
    size_ = 0;
}

bool BinaryIndexedTree::isEmpty() const { return size_ == 0; }
}  // namespace GuiBridge