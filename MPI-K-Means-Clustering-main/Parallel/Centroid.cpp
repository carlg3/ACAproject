#include "Centroid.h"

using namespace std;

list<Centroid*> Centroid::centroids;

Centroid::Centroid(int dim) : Tupla(dim) {
    centroids.push_back(this);
}

// UNUSED FUNCTION
Centroid* Centroid::getThCentroid(int index){
    auto it = centroids.begin();
    advance(it,index);
    return *it;
}

// UNUSED FUNCTION
int Centroid::getNumberCentroids(){
    return centroids.size();
}