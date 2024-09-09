//
// Created by galan on 08/09/2024.
//

#ifndef CENTROID_H
#define CENTROID_H

#include <list>
#include "Tupla.h"
using namespace std;

class Centroid: public Tupla{
public:
    Centroid(int dim);
    static Centroid* getThCentroid(int index);
    static int getNumberCentroids();
private:
    static list<Centroid*> centroids;
};

#endif //CENTROID_H
