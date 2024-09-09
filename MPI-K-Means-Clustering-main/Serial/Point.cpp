//
// Created by galan on 08/09/2024.
//

#include "Point.h"
using namespace std;

list<Point*> Point::points;

Point::Point(int dim) : Tupla(dim) {
    points.push_back(this);
}

Point* Point::getThPoint(int index){
    auto it = points.begin();
    advance(it, index);
    return *it;
}

int Point::getNumberPoints(){
    return points.size();
}
