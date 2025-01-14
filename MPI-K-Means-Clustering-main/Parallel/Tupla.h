#ifndef PARALLEL_TUPLA_H
#define PARALLEL_TUPLA_H

#include <string>
#include <vector>
using namespace std;

class Tupla{
public:
    Tupla(int dim);
    int get_dim();
    double get_value(int i) const;
    void set_value(int i, double d);
    void set_tupla(const vector<double>& d);
    double distanza(const Tupla& t) const;
    string toString();
protected:
    vector<double> value;
    int dim;
};
#endif //PARALLEL_TUPLA_H
