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
    sumCluster = new double[centroidDimension];
    for(int i=0;i<centroid->getDim();i++){
        sumCluster[i] = 0;
    }
    Cluster::sumDistance = 0;
}

void Cluster::createKclusters(int K,int centroidDimension){
    for(int i=0;i<K;i++){
        new Cluster(centroidDimension);
    }
}

void Cluster::createCentroid(int centroidDimension){
    centroid = new Centroid(centroidDimension);
    int n = rand() % (Point::getNumberPoints()-1); // Choose the value of the centroid among the points in the dataset
    for(int j=0; j < centroidDimension; j++){
        centroid->setThValue(j,Point::getThPoint(n)->getThValue(j));
        //centroid->setThValue(j,rand() % 10);
    }
}

void Cluster::setEmptyCluster(){
    points.clear();
    numberElements = 0;
    for(int i=0;i<centroid->getDim();i++){
        sumCluster[i] = 0;
    }
    Cluster::sumDistance = 0;
}

void Cluster::clustersReset(){
    for(int i=0;i<numberCluster;i++){
        Cluster::getThCluster(i)->setEmptyCluster();
    }
}

void Cluster::addElement(Point *t){
    points.push_back(t);
    numberElements++;
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

void Cluster::pointAssignment(int startIndex, int endIndex){
    int distanzaMinimaIndex;
    double distanzaMinima;
    double next;
    double d[Cluster::getNumberCluster()]; // Buffer to store the distance between the point and the centroid
    for(int i=startIndex;i<endIndex;i++){
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

void Cluster::sumPoints(){
    for(int i=0;i<Cluster::getThCluster(0)->getCentroid()->getDim();i++){
        sumCluster[i] = 0;
    }
    for(int i=0;i<numberElements;i++){
        for(int j=0;j<centroid->getDim();j++){
            sumCluster[j] = sumCluster[j] + getThPoint(i)->getThValue(j);
        }
    }
}

void Cluster::sumPointsClusters(){
    for(int i=0;i<Cluster::getNumberCluster();i++){
        Cluster::getThCluster(i)->sumPoints();
    }
}

void Cluster::centroidParallelCalculator(){
    if(numberElements){
        for(int i=0;i<centroid->getDim();i++){
            centroid->setThValue(i, sumCluster[i]/numberElements);
        }
    }else{
        for(int i=0;i<centroid->getDim();i++) {
            centroid->setThValue(i,0);
        }
    }
}

void Cluster::centroidsParallelAssignment(){
    for(int i=0;i<Cluster::getNumberCluster();i++){
        Cluster::getThCluster(i)->centroidParallelCalculator();
    }
}

double Cluster::totalMSE(){
    return Cluster::sumDistance/Point::getNumberPoints();
}

void Cluster::serializeCluster(double* buffer, int k, int dim){
    buffer[0] = k; // Number of klusters
    buffer[1] = dim; // Centroid dimension
    for(int i=0;i<buffer[0];i++) {
        for(int j=0;j<dim;j++){
            buffer[i*dim+2+j] = Cluster::getThCluster(i)->getCentroid()->getThValue(j);
        }
    }
}

void Cluster::deserializeCluster(double* buffer){
    //int numberClusters = buffer[0];// Total number of clusters received
    int dim = buffer[1]; // Cluster dimension
    Cluster::createKclusters(buffer[0],dim);
    for(int i=0;i<buffer[0];i++) {
        for(int j=0;j<dim;j++){
            Cluster::getThCluster(i)->getCentroid()->setThValue(j,buffer[i*dim+2+j]);
        }
    }
}

void Cluster::serializeCentroids(double* buffer){
    for(int i=0;i<Cluster::getNumberCluster();i++){
        for(int j=0;j<Cluster::getThCluster(i)->getCentroid()->getDim();j++){
            buffer[i*Cluster::getThCluster(i)->getCentroid()->getDim()+j] = Cluster::getThCluster(i)->getCentroid()->getThValue(j);
        }
    }
}

void Cluster::deSerializeCentroids(double* buffer) {
    for(int i=0;i<Cluster::getNumberCluster();i++){
        for(int j=0;j<Cluster::getThCluster(i)->getCentroid()->getDim();j++){
            Cluster::getThCluster(i)->getCentroid()->setThValue(j,buffer[i*Cluster::getThCluster(i)->getCentroid()->getDim()+j]);
        }
    }
}

void Cluster::serializeSumClusters(double* buffer){
    for(int i=0;i<Cluster::getNumberCluster();i++){
        for(int j=0;j<Cluster::getThCluster(i)->getCentroid()->getDim();j++){
            buffer[i*(Cluster::getThCluster(i)->getCentroid()->getDim()+1)+j] = Cluster::getThCluster(i)->getSumCluster(j);
        }
        buffer[i*(Cluster::getThCluster(i)->getCentroid()->getDim()+1)+Cluster::getThCluster(i)->getCentroid()->getDim()] = Cluster::getThCluster(i)->getNumberElements();
    }
}

void Cluster::printClusters(){
    for(int i=0;i<numberCluster;i++) {
        cout << "CLUSTER NUMBER = " << Cluster::getThCluster(i)->getNumberElements() << endl;
        cout << "CENTROID = " << Cluster::getThCluster(i)->getCentroid()->toString() << endl;
        for(int j=0;j<Cluster::getThCluster(i)->getNumberElements();j++){
            cout << Cluster::getThCluster(i)->getThPoint(j)->toString() << endl;
        }
    }
    cout << "---------------------" << endl;
}

void Cluster::printCentroids(){
    for(int i=0;i<numberCluster;i++) {
        cout << "CENTROID = " << Cluster::getThCluster(i)->getCentroid()->toString() << endl;
        cout << "sumCluster = ";
        Cluster::getThCluster(i)->printSum();
        cout << endl;
        cout << "numberElements = " << Cluster::getThCluster(i)->getNumberElements() << endl;
    }
    cout << "---------------------" << endl;
}

void Cluster::printSum(){
    string s = "<";
    for(int i=0;i<centroid->getDim();i++){
        if(i==0){
            s = s + to_string(sumCluster[i]);
        }else{
            s = s + "," + to_string(sumCluster[i]);
        }
    }
    s = s + ">";
    cout << s;
}

Cluster* Cluster::getThCluster(int index){
    auto it = clusters.begin();
    advance(it,index);
    return *it;
}

int Cluster::getNumberCluster(){
    return Cluster::numberCluster;
}

Centroid* Cluster::getCentroid(){
    return centroid;
}

int Cluster::getNumberElements(){
    return numberElements;
}

Point* Cluster::getThPoint(int index) {
    auto it = this->points.begin();
    advance(it,index);
    return *it;
}

double Cluster::getSumCluster(int index){
    return sumCluster[index];
}

void Cluster::setNumberElements(int num){
    numberElements = num;
}

void Cluster::setSumCluster(int index, double value){
    sumCluster[index] = value;
}

double Cluster::getSumDistance(){
    return Cluster::sumDistance;
}

void Cluster::setSumDistance(double value){
    Cluster::sumDistance = value;
}

void Cluster::setThCentroid(int index, double value){
    centroid->setThValue(index,value);
}

