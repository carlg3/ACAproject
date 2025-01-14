//
// Created by galan on 08/09/2024.
//

#include "Point.h"
using namespace std;

vector<Point*> Point::points;

Point::Point(int dim) : Tupla(dim) {
    points.push_back(this);
}

Point* Point::get_point(int index){
    auto it = points.begin();
    advance(it, index);
    return *it;
}

int Point::get_spoints_(){
    return points.size();
}
