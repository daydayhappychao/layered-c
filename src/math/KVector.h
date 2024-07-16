#ifndef KVECTOR_HPP
#define KVECTOR_HPP
#include <random>
#include <string>

namespace GuiBridge {
class KVector {
public:
    KVector();
    KVector(double x, double y);
    KVector(const KVector &v);
    KVector(const KVector &start, const KVector &end);
    explicit KVector(double angle);

    KVector clone();
    std::string toString();
    bool equals(const KVector &v);
    bool equalsFuzzily(const KVector &v);
    double length();
    double squareLength();
    KVector reset();
    KVector set(double x, double y);
    KVector set(const KVector &v);
    KVector add(double dx, double dy);
    KVector add(const KVector &v);
    KVector sum(const std::initializer_list<KVector> &vs);
    KVector sub(double dx, double dy);
    KVector sub(const KVector &v);
    KVector diff(const KVector &v1, const KVector &v2);
    KVector scale(double scale);
    KVector scale(double scaleX, double scaleY);
    KVector normalize();
    KVector scaleToLength(double length);
    KVector negate();
    double toDegress();
    double toRadians();
    void wiggle(std::mt19937 &random, double amount);
    double distance(const KVector &v2);
    double dotProduct(const KVector &v2);
    double crossProduct(const KVector &v, const KVector &w);
    KVector rotate(double angle);
    double angle(KVector &v);
    KVector bound(double minX, double minY, double maxX, double maxY);
    bool isNaN();
    bool isInfinite();
    void parse(std::string s);
    bool isdelim(char c, const std::string &delim);
    double x;
    double y;

private:
    double DEFAULT_FUZZYNESS = 0.05;
};
}  // namespace GuiBridge
#endif