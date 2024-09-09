//
// Created by galan on 08/09/2024.
//

#include "Centroid.h"
using namespace std;

list<Centroid*> Centroid::centroids;

Centroid::Centroid(int dim) : Tupla(dim) {
    centroids.push_back(this);
}

Centroid* Centroid::getThCentroid(int index){
    auto it = centroids.begin();
    advance(it, index);
    return *it;
}

int Centroid::getNumberCentroids(){
    return centroids.size();
}
