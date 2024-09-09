#ifndef PARALLEL_TUPLA_H
#define PARALLEL_TUPLA_H

#include <string>
#include <vector>
using namespace std;

class Tupla{
public:
    Tupla(int dim);
    Tupla differenzaVettoriale(const Tupla& t) const;
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
#endif //PARALLEL_TUPLA_H
