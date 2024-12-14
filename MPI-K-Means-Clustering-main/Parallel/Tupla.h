#ifndef PARALLEL_TUPLA_H
#define PARALLEL_TUPLA_H

#include <string>
#include <vector>
using namespace std;

class Tupla{
public:
    Tupla(int dim);
    int getDim();
    double getThValue(int i) const;
    void setThValue(int i, double d);
    void setValue(const vector<double>& d);
    double distanza(const Tupla& t) const;
    string toString();
protected:
    int dim;
    vector<double> value;
};
#endif //PARALLEL_TUPLA_H
