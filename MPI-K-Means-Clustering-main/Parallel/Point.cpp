#include <iostream>
#include "Point.h"

using namespace std;

int Point::numberPoints;
list<Point*> Point::points;

Point::Point(int dim) : Tupla(dim) {
    points.push_back(this);
    numberPoints++;
}

Point* Point::getThPoint(int index){
    auto it = points.begin();
    advance(it,index);
    return *it;
}

int Point::getNumberPoints(){
    return numberPoints;
}

void Point::printPoints(){
    for(int i=0;i<getNumberPoints();i++){
        cout << getThPoint(i)->toString() << endl;
    }
}

void Point::serializePoint(double* buffer,int startIndex, int endIndex, int dim){
    buffer[0] = (endIndex-startIndex); // Total number of point to send
    buffer[1] = dim; // Point dimension
    for(int i=0;i<buffer[0];i++){
        for(int j=0;j<dim;j++){
            buffer[i*dim+2+j] = Point::getThPoint(i+startIndex)->getThValue(j);
        }
    }
}

void Point::deserializaPoint(double* buffer){
    //int numberPoints = buffer[0];// Total number of point received
    int dim = buffer[1]; // Point dimension
    for(int i=0;i<buffer[0];i++) {
        Point *p = new Point(dim);
        for(int j=0;j<dim;j++){
            p->setThValue(j,buffer[i*dim+2+j]);
        }
    }
}