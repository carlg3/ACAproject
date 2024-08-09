#include <list>
#include <iostream>

#include "Tupla.h"
#include "Cluster.h"

using namespace std;

double Cluster::sumDistance;
int Cluster::numberCluster;
list<Cluster*> Cluster::clusters;

Cluster::Cluster(int centroidDimension){
    this->numberElements = 0;
    clusters.push_back(this);
    numberCluster++;
    createCentroid(centroidDimension);
    Cluster::sumDistance = 0;
}

void Cluster::setEmptyCluster(){
    points.clear();
    numberElements = 0;
    Cluster::sumDistance = 0;
}

void Cluster::createKclusters(int K,int centroidDimension){
    for(int i=0;i<K;i++){
        new Cluster(centroidDimension);
    }
}

void Cluster::createCentroid(int centroidDimension){
    centroid = new Centroid(centroidDimension);
    int n = rand() % (Point::getNumberPoints()-1); // Scegliere il punto a cui assegnare il centroide tra i punti del dataset
    for(int j=0; j < centroidDimension; j++){
        centroid->setThValue(j,Point::getThPoint(n)->getThValue(j));
        //centroid->setThValue(j,rand() % 10);
    }
}

Cluster* Cluster::getThCluster(int index){
    auto it = clusters.begin();
    advance(it,index);
    return *it;
}

int Cluster::getNumberCluster(){
    return Cluster::numberCluster;
}

void Cluster::addElement(Point *t){
    points.push_back(t);
    numberElements++;
}

void Cluster::centroidCalculator(){
    if(numberElements){
        for(int i=0;i<centroid->getDim();i++){
                    centroid->setThValue(i, meanCalculator(i));
        }
    }else{
        for(int i=0;i<centroid->getDim();i++) {
            centroid->setThValue(i,0);
        }
    }
}

double Cluster::meanCalculator(int index){
    double sum = 0;
    for(int j=0;j<numberElements;j++){
            sum = sum + this->getThPoint(j)->getThValue(index);
    }
    return sum/numberElements;
}

Point* Cluster::getElementList(int index){
    auto it = this->points.begin();
    advance(it,index);
    return *it;
}

int Cluster::getNumberElements(){
    return numberElements;
}

Centroid* Cluster::getCentroid(){
    return centroid;
}

Point* Cluster::getThPoint(int index) {
    auto it = points.begin();
    advance(it, index);
    return *it;
}

void Cluster::clustersReset(){
    for(int i=0;i<numberCluster;i++){
        Cluster::getThCluster(i)->setEmptyCluster();
    }
}

void Cluster::pointAssignment(){
    int distanzaMinimaIndex;
    double distanzaMinima;
    double next;
    double d[Cluster::getNumberCluster()]; // Buffer to store the distance between the point and the centroid
    for(int i=0;i<Point::getNumberPoints();i++){
        distanzaMinimaIndex = 0;
        distanzaMinima = Point::getThPoint(i)->distanza(*Cluster::getThCluster(0)->getCentroid());
        d[0] = distanzaMinima; // Store the distance between the point and the centroid
        for(int j=1;j<Cluster::getNumberCluster();j++){
            next = Point::getThPoint(i)->distanza(*Cluster::getThCluster(j)->getCentroid());
            d[j] = next; // Store the distance between the point and the centroid
            if(next<distanzaMinima){
                distanzaMinima = next;
                distanzaMinimaIndex = j;
            }
        }
        Cluster::getThCluster(distanzaMinimaIndex)->addElement(Point::getThPoint(i));
        Cluster::sumDistance += d[distanzaMinimaIndex];
    }
}

void Cluster::centroidsAssignment(){
    for(int i=0;i<numberCluster;i++){
        Cluster::getThCluster(i)->centroidCalculator();
    }
}

double Cluster::totalMSE(){
    return Cluster::sumDistance/Point::getNumberPoints();
}

void Cluster::printClusters(){
    for(int i=0;i<numberCluster;i++) {
        cout << "CLUSTER NUMBER = " << Cluster::getThCluster(i)->getNumberElements() << endl;
        cout << "CENTROID = " << Cluster::getThCluster(i)->getCentroid()->toString() << endl;
        for(int j=0;j<Cluster::getThCluster(i)->getNumberElements();j++){
            cout << Cluster::getThCluster(i)->getElementList(j)->toString() << endl;
        }
    }
    cout << "---------------------" << endl;
}

void Cluster::setThCentroid(int index, double value){
    centroid->setThValue(index,value);
}
