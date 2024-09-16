<<<<<<< Updated upstream
#include "Centroid.h"

using namespace std;

vector<Centroid*> Centroid::centroids;

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
=======
#include "Centroid.h"

using namespace std;

int Centroid::numberCentroids;
list<Centroid*> Centroid::centroids;

Centroid::Centroid(int dim) : Tupla(dim) {
    centroids.push_back(this);
    numberCentroids++;
}

// UNUSED FUNCTION
Centroid* Centroid::getThCentroid(int index){
    auto it = centroids.begin();
    advance(it,index);
    return *it;
}

// UNUSED FUNCTION
int Centroid::getNumberCentroids(){
    return numberCentroids;
>>>>>>> Stashed changes
}