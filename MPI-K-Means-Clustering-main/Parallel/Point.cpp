#include <iostream>
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

void Point::print_points_(){
    for(auto point : points){
        cout << point->toString() << endl;
    }
}

void Point::serializePoint(double* buffer, int startIndex, int endIndex, int dim){
    buffer[0] = endIndex - startIndex; // Total number of point to send
    buffer[1] = dim; // Point dimension

    for(int i = 0; i < buffer[0]; i++){
        for(int j = 0; j < dim; j++){
            buffer[i * dim + j + 2] = Point::get_point(i + startIndex)->get_value(j);
        }
    }
}

void Point::deserializePoint(double* buffer){
    int elements = buffer[0];
    int dim = buffer[1]; // Point dimension

    for(int i = 0; i < elements; i++) {
        Point *p = new Point(dim);

        for(int j = 0; j < dim; j++){
            p->set_value(j, buffer[i * dim + j + 2]);
        }
    }
}