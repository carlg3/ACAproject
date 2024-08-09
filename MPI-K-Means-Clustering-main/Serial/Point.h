#ifndef ACAPROJECT_POINT_H
#define ACAPROJECT_POINT_H

#include <list>

#include "Tupla.h"

class Point: public Tupla{
    public:
    Point(int dim);
    static Point* getThPoint(int index);
    static int getNumberPoints();
    private:
    static int numberPoints;
    static std::list<Point*> points;
};

#endif //ACAPROJECT_POINT_H
