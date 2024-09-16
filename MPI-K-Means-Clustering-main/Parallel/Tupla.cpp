<<<<<<< Updated upstream
#include "Tupla.h"
#include <iostream>
#include <vector>
using namespace std;

Tupla::Tupla(int dim) : dim(dim), value(dim) {}

double Tupla::distanza(const Tupla& t) const {
    Tupla differenza = this->differenzaVettoriale(t);
    double result = 0;
    for (int i = 0; i < dim; i++) {
        result += differenza.getThValue(i) * differenza.getThValue(i);
    }
    return result;
}

Tupla Tupla::differenzaVettoriale(const Tupla& t) const {
    Tupla v(dim);
    for (int i = 0; i < dim; i++) {
        v.setThValue(i, this->getThValue(i) - t.getThValue(i));
    }
    return v;
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
=======
#include "Tupla.h"
#include <string>
#include <math.h>

using namespace std;

Tupla::Tupla(int dim){
    value = new double[dim];
    this->dim = dim;
}

double Tupla::distanza(Tupla t){
    Tupla differenza = this->differenzaVettoriale(t);
    double result = 0;
    for(int i=0;i<dim;i++){
        result = result + pow(differenza.getThValue(i),2);
    }
    return result;
}

Tupla Tupla::differenzaVettoriale(Tupla t){
    Tupla *v = new Tupla(dim);
    for(int i=0;i<dim;i++){
        v->setThValue(i,this->getThValue(i) - t.getThValue(i));
    }
    return *v;
}

double Tupla::getThValue(int i){
    return value[i];
}

void Tupla::setThValue(int i, double d){
    value[i] = d;
}

int Tupla::getDim(){
    return dim;
}

string Tupla::toString(){
    string s = "<";
    for(int i=0;i<dim;i++){
        if(i==0){
            s = s + to_string(value[i]);
        }else{
            s = s + "," + to_string(value[i]);
        }
    }
    return s + ">";
}

>>>>>>> Stashed changes
