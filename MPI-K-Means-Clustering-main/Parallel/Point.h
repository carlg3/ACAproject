#ifndef PARALLEL_POINT_H
#define PARALLEL_POINT_H

#include "Tupla.h"
using namespace std;

class Point: public Tupla{
public:
    Point(int dim);
    static Point* getThPoint(int index);
    static int getTotalNumberPoints();
    static void serializePoint(double* buffer,int startIndex, int endIndex, int dim);
    static void deserializePoint(double* buffer);
    static void printPoints();
private:
    // static int numberPoints;
    static vector<Point*> points;
};

#endif //PARALLEL_POINT_H
