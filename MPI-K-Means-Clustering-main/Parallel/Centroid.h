#ifndef PARALLEL_CENTROID_H
#define PARALLEL_CENTROID_H

#include <list>

#include "Tupla.h"

class Centroid: public Tupla{
public:
    Centroid(int dim);
    static Centroid* getThCentroid(int index); // UNUSED FUNCTION
    static int getNumberCentroids(); // UNUSED FUNCTION
private:
    static int numberCentroids;
    static std::list<Centroid*> centroids;
};

#endif //PARALLEL_CENTROID_H
