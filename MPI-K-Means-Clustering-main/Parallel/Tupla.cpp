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

