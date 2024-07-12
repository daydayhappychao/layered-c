#include "KVectorChain.h"
#include <cmath>
#include <sstream>
#include <string>
#include <vector>
#include "../utils/StringUtil.h"
#include "./KVector.h"
namespace GuiBridge {
KVectorChain::KVectorChain() : std::vector<KVector>() {}

KVectorChain::KVectorChain(const std::vector<KVector> &collection) : std::vector<KVector>(collection) {}

KVectorChain::KVectorChain(const std::initializer_list<KVector> &vectors) : std::vector<KVector>(vectors) {}

std::string KVectorChain::toString() const {
    std::stringstream builder;
    builder << "(";
    for (auto it = begin(); it != end(); ++it) {
        auto vector = *it;

        builder << vector.x << "," << vector.y;
        if (std::next(it) != end()) {
            builder << "; ";
        }
    }
    builder << ")";
    return builder.str();
}

std::vector<KVector> KVectorChain::toArray() const {
    std::vector<KVector> result(size());
    std::copy(begin(), end(), result.begin());
    return result;
}

std::vector<KVector> KVectorChain::toArray(int beginIndex) const {
    if (beginIndex < 0 || beginIndex > size()) {
        throw std::out_of_range("Index out of bounds");
    }
    std::vector<KVector> result(size() - beginIndex);
    std::copy(begin() + beginIndex, end(), result.begin());
    return result;
}

void KVectorChain::parse(const std::string &string) {
    std::vector<std::string> tokens = split(string, ",|;|\\(|\\)|\\[|\\]|\\{|\\}| |\t|\n");
    clear();

    try {
        int xy = 0;
        double x = 0;
        double y = 0;
        for (auto token : tokens) {
            if (trim(token).length() > 0) {
                if (xy % 2 == 0) {
                    x = std::stod(token);
                } else {
                    y = std::stod(token);
                }

                if (xy > 0 && xy % 2 == 1) {
                    push_back(KVector(x, y));
                }
                xy++;
            }
        }
    } catch (const std::invalid_argument &e) {
        throw std::invalid_argument("The given string does not match the expected format for vectors.");
    }
}

void KVectorChain::add() { push_back(KVector()); }

void KVectorChain::add(double x, double y) { push_back(KVector(x, y)); }

void KVectorChain::addFirst() { insert(begin(), KVector()); }

void KVectorChain::addFirst(double x, double y) { insert(begin(), KVector(x, y)); }

void KVectorChain::addLast() { push_back(KVector()); }

void KVectorChain::addLast(double x, double y) { push_back(KVector(x, y)); }

void KVectorChain::addAll(const std::vector<KVector> &vectors) { insert(end(), vectors.begin(), vectors.end()); }

void KVectorChain::addAllAsCopies(int index, const std::vector<KVector> &chain) {
    std::vector<KVector> copies;
    for (const KVector &v : chain) {
        copies.push_back(KVector(v));
    }
    insert(begin() + index, copies.begin(), copies.end());
}

KVectorChain &KVectorChain::scale(double scale) {
    for (KVector &vector : *this) {
        vector.scale(scale);
    }
    return *this;
}

KVectorChain &KVectorChain::scale(double scalex, double scaley) {
    for (KVector &vector : *this) {
        vector.scale(scalex, scaley);
    }
    return *this;
}

KVectorChain &KVectorChain::offset(const KVector &offset) {
    for (KVector &vector : *this) {
        vector.add(offset);
    }
    return *this;
}

KVectorChain &KVectorChain::offset(double dx, double dy) {
    for (KVector &vector : *this) {
        vector.add(dx, dy);
    }
    return *this;
}

double KVectorChain::totalLength() const {
    double length = 0;
    if (size() >= 2) {
        auto it = begin();
        KVector point1 = *it++;
        while (it != end()) {
            KVector point2 = *it++;
            length += point1.distance(point2);
            point1 = point2;
        }
    }
    return length;
}

bool KVectorChain::hasNaN() const {
    for (const KVector &v : *this) {
        if (std::isnan(v.x) || std::isnan(v.y)) {
            return true;
        }
    }
    return false;
}

bool KVectorChain::hasInfinite() const {
    for (const KVector &v : *this) {
        if (std::isinf(v.x) || std::isinf(v.y)) {
            return true;
        }
    }
    return false;
}

KVector KVectorChain::pointOnLine(double dist) const {
    if (size() >= 2) {
        double absDistance = std::abs(dist);
        double distanceSum = 0;
        if (dist >= 0) {
            auto it = begin();
            KVector currentPoint = *it++;
            while (it != end()) {
                double oldDistanceSum = distanceSum;
                KVector nextPoint = *it++;
                double additionalDistanceToNext = currentPoint.distance(nextPoint);
                if (additionalDistanceToNext > 0) {
                    distanceSum += additionalDistanceToNext;
                    if (distanceSum >= absDistance) {
                        double thisRelative = (absDistance - oldDistanceSum) / additionalDistanceToNext;
                        KVector result = nextPoint.clone().sub(currentPoint);
                        result.scale(thisRelative);
                        result.add(currentPoint);
                        return result;
                    }
                }
                currentPoint = nextPoint;
            }
            return currentPoint;
        } else {
            auto it = end();
            KVector currentPoint = *--it;
            while (it != begin()) {
                double oldDistanceSum = distanceSum;
                KVector nextPoint = *--it;
                double additionalDistanceToNext = currentPoint.distance(nextPoint);
                if (additionalDistanceToNext > 0) {
                    distanceSum += additionalDistanceToNext;
                    if (distanceSum >= absDistance) {
                        double thisRelative = (absDistance - oldDistanceSum) / additionalDistanceToNext;
                        KVector result = nextPoint.clone().sub(currentPoint);
                        result.scale(thisRelative);
                        result.add(currentPoint);
                        return result;
                    }
                }
                currentPoint = nextPoint;
            }
            return currentPoint;
        }
    } else if (size() == 1) {
        return front();
    } else {
        throw std::logic_error("Cannot determine a point on an empty vector chain.");
    }
}

double KVectorChain::angleOnLine(double dist) const {
    if (size() >= 2) {
        double absDistance = std::abs(dist);
        double distanceSum = 0;
        if (dist >= 0) {
            auto it = begin();
            KVector currentPoint = *it++;
            while (it != end()) {
                KVector nextPoint = *it++;
                double additionalDistanceToNext = currentPoint.distance(nextPoint);
                if (additionalDistanceToNext > 0) {
                    distanceSum += additionalDistanceToNext;
                    if (distanceSum >= absDistance) {
                        return nextPoint.clone().sub(currentPoint).toRadians();
                    }
                }
                currentPoint = nextPoint;
            }
        } else {
            auto it = end();
            KVector currentPoint = *--it;
            while (it != begin()) {
                KVector nextPoint = *--it;
                double additionalDistanceToNext = currentPoint.distance(nextPoint);
                if (additionalDistanceToNext > 0) {
                    distanceSum += additionalDistanceToNext;
                    if (distanceSum >= absDistance) {
                        return nextPoint.clone().sub(currentPoint).toRadians();
                    }
                }
                currentPoint = nextPoint;
            }
        }
    }
    throw std::logic_error("Need at least two points to determine an angle.");
}

KVectorChain KVectorChain::reverse(const KVectorChain &chain) {
    KVectorChain result;
    for (auto it = chain.rbegin(); it != chain.rend(); ++it) {
        result.push_back(*it);
    }
    return result;
}
}  // namespace GuiBridge