//
// Created by galan on 08/09/2024.
//

#ifndef TUPLA_H
#define TUPLA_H

#include <string>
#include <vector>
using namespace std;

class Tupla{
public:
    Tupla(int dim);
    double distanza(const Tupla& t) const;
    void set_value(int i, double d);
    void set_tupla(const vector<double>& d);
    double get_value(int i) const;
    int get_dim();
    string toString();
protected:
    vector<double> value;
    int dim;
};

#endif //TUPLA_H
