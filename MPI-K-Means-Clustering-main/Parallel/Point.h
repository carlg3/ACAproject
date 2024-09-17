#ifndef PARALLEL_POINT_H
#define PARALLEL_POINT_H

#include <list>

#include "Tupla.h"

class Point: public Tupla{
public:
    Point(int dim);
    static Point* getThPoint(int index);
    static int getNumberPoints();
    static void printPoints();
    static void serializePoint(double* buffer,int startIndex, int endIndex, int dim);
    static void deserializaPoint(double* buffer);
private:
    static int numberPoints;
    static std::list<Point*> points;
};

#endif //PARALLEL_POINT_H
