#ifndef ACAPROJECT_CENTROID_H
#define ACAPROJECT_CENTROID_H

#include <list>

#include "Tupla.h"

class Centroid: public Tupla{
    public:
    Centroid(int dim);
    static Centroid* getThCentroid(int index);
    static int getNumberCentroids();
    private:
    static int numberCentroids;
    static std::list<Centroid*> centroids;
};

#endif //ACAPROJECT_CENTROID_H
