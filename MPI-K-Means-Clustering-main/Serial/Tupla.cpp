//
// Created by galan on 08/09/2024.
//

#include "Tupla.h"
#include <iostream>
#include <vector>
using namespace std;

Tupla::Tupla(int dim) : value(dim), dim(dim) {}

double Tupla::distanza(const Tupla& t) const {
    double result = 0;
    for (int i = 0; i < dim; i++) {
        double diff = this->get_value(i) - t.get_value(i);
        result += diff * diff;
    }
    return result;
}

double Tupla::get_value(int i) const {
    return value.at(i);
}

void Tupla::set_value(int i, double d) {
    value.at(i) = d;
}

void Tupla::set_tupla(const vector<double>& d) {
    value = d;
}

int Tupla::get_dim() {
    return dim;
}

string Tupla::toString() {
    // TEST
    string s = "(";
    for (int i = 0; i < dim; i++) {
        if (i == 0) {
            s += to_string(value.at(i));
            // s += to_string(static_cast<int>(value.at(i)));
        } else {
            s += "," + to_string(value.at(i));
            // s += "," + to_string(static_cast<int>(value.at(i)));
        }
    }
    return s + ")";
}
