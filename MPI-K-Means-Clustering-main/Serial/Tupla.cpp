//
// Created by galan on 08/09/2024.
//

#include "Tupla.h"
#include <iostream>
#include <vector>
using namespace std;

Tupla::Tupla(int dim) : dim(dim), value(dim) {}

double Tupla::distanza(const Tupla& t) const {
    double result = 0;
    for (int i = 0; i < dim; i++) {
        double diff = this->getThValue(i) - t.getThValue(i);
        result += diff * diff;
    }
    return result;
}

double Tupla::getThValue(int i) const {
    return value.at(i);
}

void Tupla::setThValue(int i, double d) {
    value.at(i) = d;
}

void Tupla::setValue(const vector<double>& d) {
    value = d;
}

int Tupla::getDim() {
    return dim;
}

string Tupla::toString() {
    string s = "(";
    for (int i = 0; i < dim; i++) {
        if (i == 0) {
            s += to_string(static_cast<int>(value.at(i)));
        } else {
            s += "," + to_string(static_cast<int>(value.at(i)));
        }
    }
    return s + ")";
}
