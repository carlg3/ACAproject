#include <iostream>
#include "Point.h"

using namespace std;

int Point::numberPoints;
list<Point*> Point::points;

Point::Point(int dim) : Tupla(dim) {
    points.push_back(this);
    // numberPoints++;
}

Point* Point::getThPoint(int index){
    auto it = points.begin();
    advance(it,index);
    return *it;
}

int Point::getNumberPoints(){
    return points.size();
}

void Point::printPoints(){
    for(auto point : points){
        cout << point->toString() << endl;
    }
}

void Point::serializePoint(double* buffer, int startIndex, int endIndex, int dim) {
    buffer[0] = (endIndex - startIndex);    // Total number of points to send
    buffer[1] = dim;                        // Point dimension

    int index = 2;
    for (int i = startIndex; i < endIndex; i++) {
        for (int j = 0; j < dim; j++) {
            buffer[index++] = Point::getThPoint(i)->getThValue(j);
        }
    }
}

void Point::deserializePoint(double* buffer) {
    // [0] Total number of points received & [1] Point dimension

    int index = 2;
    for (int i = 0; i < buffer[0]; i++) {
        Point *p = new Point(buffer[0]);

        for (int j = 0; j < buffer[1]; j++) {
            p->setThValue(j, buffer[index++]);
        }
    }
}