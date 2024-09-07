#include "Point.h"

int Point::numberPoints = 0;
std::list<Point*> Point::points;

Point::Point(int dim) : Tupla(dim) {
    points.push_back(this);
    numberPoints++;
}

Point* Point::getThPoint(int index){
    auto it = points.begin();
    std::advance(it, index);
    return *it;
}

int Point::getNumberPoints(){
    return numberPoints;
}
