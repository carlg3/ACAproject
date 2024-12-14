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
    void setThValue(int i, double d);
    void setValue(const vector<double>& d);
    double getThValue(int i) const;
    int getDim();
    string toString();
protected:
    int dim;
    vector<double> value;
};

#endif //TUPLA_H
