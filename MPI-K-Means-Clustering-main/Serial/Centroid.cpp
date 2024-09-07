#include <cmath>
#include "Centroid.h"

int Centroid::numberCentroids = 0;
std::list<Centroid*> Centroid::centroids;

Centroid::Centroid(int dim) : Tupla(dim) {
    centroids.push_back(this);
    numberCentroids++;
}

Centroid* Centroid::getThCentroid(int index){
    auto it = centroids.begin();
    std::advance(it, index);
    return *it;
}

int Centroid::getNumberCentroids(){
    return numberCentroids;
}
