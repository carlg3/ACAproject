//
// Created by galan on 08/09/2024.
//

#ifndef POINT_H
#define POINT_H

#include <list>
#include "Tupla.h"
using namespace std;

class Point: public Tupla{
public:
    Point(int dim);
    static Point* getThPoint(int index);
    static int getNumberPoints();
private:
    static vector<Point*> points;
};

#endif //POINT_H
