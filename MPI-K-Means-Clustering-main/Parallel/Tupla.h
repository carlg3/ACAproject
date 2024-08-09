#ifndef PARALLEL_TUPLA_H
#define PARALLEL_TUPLA_H

#include <string>

class Tupla{
public:
    Tupla(int dim);
    double distanza(Tupla t);
    Tupla differenzaVettoriale(Tupla t);
    double getThValue(int i);
    void setThValue(int i, double d);
    int getDim();
    std::string toString();
protected:
    double* value;
    int dim;
};

#endif //PARALLEL_TUPLA_H
