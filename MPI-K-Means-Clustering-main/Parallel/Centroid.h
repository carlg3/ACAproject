#ifndef PARALLEL_CENTROID_H
#define PARALLEL_CENTROID_H

#include "Tupla.h"
using namespace std;

class Centroid: public Tupla{
public:
    Centroid(int dim);
    static Centroid* get_centroid(int index);
    static int get_scentroids_();
private:
    static vector<Centroid*> centroids;
};

#endif //PARALLEL_CENTROID_H