#include "Centroid.h"

using namespace std;

int Centroid::numberCentroids;
list<Centroid*> Centroid::centroids;

Centroid::Centroid(int dim) : Tupla(dim) {
    centroids.push_back(this);
    numberCentroids++;
}

Centroid* Centroid::getThCentroid(int index){
    auto it = centroids.begin();
    advance(it,index);
    return *it;
}

int Centroid::getNumberCentroids(){
    return numberCentroids;
}