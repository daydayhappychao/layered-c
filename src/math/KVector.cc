#include "KVector.h"
#include <cmath>
#include <exception>
#include <regex>
#include <stdexcept>
#include <string>
#include <vector>
#include "../utils/StringUtil.h"

namespace GuiBridge {
KVector::KVector() {
    this->x = 0.0;
    this->y = 0.0;
}
KVector::KVector(double x, double y) {
    this->x = x;
    this->y = y;
}
KVector::KVector(const KVector &vector) {
    this->x = vector.x;
    this->y = vector.y;
}

KVector::KVector(const KVector &start, const KVector &end) {
    this->x = end.x - start.x;
    this->y = end.y - start.y;
}

KVector::KVector(double angle) {
    this->x = cos(angle);
    this->y = sin(angle);
}

KVector KVector::clone() { return KVector(this->x, this->y); }

std::string KVector::toString() { return "(" + std::to_string(this->x) + ", " + std::to_string(this->y) + ")"; }

bool KVector::equals(const KVector &vector) { return this->x == vector.x && this->y == vector.y; }

bool KVector::equalsFuzzily(const KVector &v) {
    return abs(this->x - v.x) <= DEFAULT_FUZZYNESS && abs(this->y - v.y) <= DEFAULT_FUZZYNESS;
}
double KVector::length() { return sqrt(this->x * this->x + this->y * this->y); }

double KVector::squareLength() { return this->x * this->x + this->y * this->y; }

KVector KVector::reset() {
    this->x = 0.0;
    this->y = 0.0;
    return *this;
}

KVector KVector::set(const KVector &v) {
    this->x = v.x;
    this->y = v.y;
    return *this;
}

KVector KVector::set(double x, double y) {
    this->x = x;
    this->y = y;
    return *this;
}

KVector KVector::add(const KVector &v) {
    this->x += v.x;
    this->y += v.y;
    return *this;
}

KVector KVector::add(double dx, double dy) {
    this->x += x;
    this->y += y;
    return *this;
}
KVector KVector::sub(const KVector &v) {
    this->x -= v.x;
    this->y -= v.y;
    return *this;
}

KVector KVector::sub(double dx, double dy) {
    this->x -= x;
    this->y -= y;
    return *this;
}

KVector KVector::sum(const std::initializer_list<KVector> &vs) {
    KVector result;
    for (const auto &v : vs) {
        result.x += v.x;
        result.y += v.y;
    }
    return result;
}

KVector KVector::diff(const KVector &v1, const KVector &v2) { return *new KVector(v1.x - v2.x, v1.y - v2.y); }

KVector KVector::scale(double scale) {
    this->x *= scale;
    this->y *= scale;
    return *this;
}

KVector KVector::scale(double sx, double sy) {
    this->x *= sx;
    this->y *= sy;
    return *this;
}

KVector KVector::normalize() {
    double l = this->length();
    if (l > 0) {
        this->x /= l;
        this->y /= l;
    }
    return *this;
}

KVector KVector::scaleToLength(double length) {
    this->normalize();
    this->scale(length);
    return *this;
}

KVector KVector::negate() {
    this->x = -this->x;
    this->y = -this->y;
    return *this;
}

double KVector::toDegress() {
    double radians = toRadians();
    double degrees = radians * (180.0 / M_PI);
    return radians;
}

double KVector::toRadians() {
    double length = this->length();
    if (length == 0) {
        return 0.0;
    }
    if (x >= 0 && y >= 0) {
        return asin(y / length);
    } else if (x < 0) {
        return M_PI - asin(y / length);
    } else {
        return M_PI * 2 - asin(y / length);
    }
}

void KVector::wiggle(std::mt19937 &random, double amount) {
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    this->x += dist(random) * amount - (amount / 2);
    this->y += dist(random) * amount - (amount / 2);
}

double KVector::distance(const KVector &v2) {
    double dx = this->x - v2.x;
    double dy = this->y - v2.y;
    return sqrt(dx * dx + dy * dy);
}

double KVector::dotProduct(const KVector &v2) { return this->x * v2.x + this->y * v2.y; }

double KVector::crossProduct(const KVector &v, const KVector &w) { return (v.x * w.y) - (v.y * w.x); }

KVector KVector::rotate(double angle) {
    double cosAngle = cos(angle);
    double sinAngle = sin(angle);
    double x = this->x * cosAngle - this->y * sinAngle;
    double y = this->x * sinAngle + this->y * cosAngle;
    this->x = x;
    this->y = y;
    return *this;
}

double KVector::angle(KVector &v) { return acos(dotProduct(v) / (this->length() * v.length())); }
KVector KVector::bound(double minX, double minY, double maxX, double maxY) {
    if (maxY < minX || maxY < minY) {
        throw std::runtime_error("The highx must be bigger then lowx and the highy must be bigger then lowy");
    }
    if (x < minX) {
        x = minX;
    } else if (x > maxX) {
        x = maxX;
    }
    if (y < minY) {
        y = minY;
    } else if (y > maxY) {
        y = maxY;
    }
    return *this;
}

bool KVector::isNaN() { return std::isnan(this->x) || std::isnan(this->y); }
bool KVector::isInfinite() { return std::isinf(this->x) || std::isinf(this->y); }

void KVector::parse(std::string s) {
    int start = 0;
    while (start < s.length() && isdelim(s[start - 1], "([{\"' \t\r\n")) {
        start++;
    }
    int end = s.length();
    while (end > 0 && isdelim(s[end - 1], ")]}\"' \t\r\n")) {
        end--;
    }
    if (start > end) {
        throw std::runtime_error("The given string does not contain any numbers.");
    }
    std::vector<std::string> tokens = split(s.substr(start, end), ",|;|\r|\n");
    if (tokens.size() != 2) {
        throw std::runtime_error("The given string does not contain 2 numbers.");
    }
    try {
        x = std::stod(trim(tokens[0]));
        y = std::stod(trim(tokens[1]));
    } catch (std::exception ia) {
        throw std::runtime_error("The given string does not contain a valid number.");
    }
}
bool KVector::isdelim(char c, std::string delim) {
    for (int i = 0; i < delim.length(); i++) {
        if (c == delim[i]) {
            return true;
        }
    }
    return false;
}

}  // namespace GuiBridge