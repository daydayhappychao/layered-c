#ifndef KVECTOR_CHAIN_H
#define KVECTOR_CHAIN_H

#include <iostream>
#include <vector>
#include "KVector.h"
namespace GuiBridge {
class KVectorChain : public std::vector<KVector> {
public:
    KVectorChain();
    explicit KVectorChain(const std::vector<KVector> &collection);
    KVectorChain(const std::initializer_list<KVector> &vectors);

    std::string toString() const;
    std::vector<KVector> toArray() const;
    std::vector<KVector> toArray(int beginIndex) const;
    void parse(const std::string &string);

    void add();
    void add(double x, double y);
    void addFirst();
    void addFirst(double x, double y);
    void addLast();
    void addLast(double x, double y);
    void addAll(const std::vector<KVector> &vectors);
    void addAllAsCopies(int index, const std::vector<KVector> &chain);

    KVectorChain &scale(double scale);
    KVectorChain &scale(double scalex, double scaley);
    KVectorChain &offset(const KVector &offset);
    KVectorChain &offset(double dx, double dy);

    double totalLength() const;
    bool hasNaN() const;
    bool hasInfinite() const;
    KVector pointOnLine(double dist) const;
    double angleOnLine(double dist) const;

    static KVectorChain reverse(const KVectorChain &chain);
};
}  // namespace GuiBridge
#endif  // KVECTOR_CHAIN_H