#ifndef PARALLEL_POINT_H
#define PARALLEL_POINT_H

#include "Tupla.h"
using namespace std;

class Point: public Tupla{
public:
    Point(int dim);
    static Point* get_point(int index);
    static int get_spoints_();

    // Serialization functions
    static void serializePoint(double* buffer,int startIndex, int endIndex, int dim);
    static void deserializePoint(double* buffer);

    // Debug
    static void print_points_();
private:
    static vector<Point*> points;
};

#endif //PARALLEL_POINT_H
