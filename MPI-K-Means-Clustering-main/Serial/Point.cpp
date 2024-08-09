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
