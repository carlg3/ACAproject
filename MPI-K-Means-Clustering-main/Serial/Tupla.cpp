#include "Tupla.h"
#include <iostream>

Tupla::Tupla(int dim) {
    value = new double[dim];
    this->dim = dim;
}

double Tupla::distanza(const Tupla& t) {
    Tupla differenza = this->differenzaVettoriale(t);
    double result = 0;
    for (int i = 0; i < dim; i++) {
        result += differenza.getThValue(i) * differenza.getThValue(i);
    }
    return result;
}

Tupla Tupla::sommaVettoriale(const Tupla& t) {
    Tupla v(dim);
    for (int i = 0; i < dim; i++) {
        v.setThValue(i, this->getThValue(i) + t.getThValue(i));
    }
    return v;
}

Tupla Tupla::differenzaVettoriale(const Tupla& t) {
    Tupla v(dim);
    for (int i = 0; i < dim; i++) {
        v.setThValue(i, this->getThValue(i) - t.getThValue(i));
    }
    return v;
}

double Tupla::getThValue(int i) const {
    return value[i];
}

void Tupla::setThValue(int i, double d) {
    value[i] = d;
}

void Tupla::setValue(double d[]) {
    value = d;
}

int Tupla::getDim() const {
    return dim;
}

std::string Tupla::toString() const {
    std::string s = "(";
    for (int i = 0; i < dim; i++) {
        if (i == 0) {
            s += std::to_string(static_cast<int>(value[i]));
        } else {
            s += "," + std::to_string(static_cast<int>(value[i]));
        }
    }
    return s + ")";
}
