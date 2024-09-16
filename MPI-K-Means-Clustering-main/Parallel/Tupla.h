<<<<<<< Updated upstream
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
    Tupla differenzaVettoriale(const Tupla& t) const;
    double distanza(const Tupla& t) const;
    string toString();
protected:
    int dim;
    vector<double> value;
};
#endif //PARALLEL_TUPLA_H
=======
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
>>>>>>> Stashed changes
