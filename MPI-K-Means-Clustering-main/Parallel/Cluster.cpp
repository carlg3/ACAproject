#include <list>
#include <iostream>
#include <fstream>

#include "Tupla.h"
#include "Cluster.h"

using namespace std;

double Cluster::sumDistance;
int Cluster::numberCluster;

list<Cluster*> Cluster::clusters;

// Crea un cluster e gli assegna un centroid
Cluster::Cluster(int centroidDimension){
    clusters.push_back(this);
    numberCluster++;

    this->points_number_ = 0;

    createCentroid(centroidDimension);

    Cluster::sumDistance = 0;

    sumCluster = new double[centroidDimension];
    for(int i = 0; i < centroid->getDim(); i++){ sumCluster[i] = 0;}
}

// ---- GETTERS & SETTERS ----
Centroid* Cluster::getCentroid(){
    return centroid;
}

Point* Cluster::getThPoint(int index) {
    auto it = points.begin();
    advance(it, index);
    return *it;
}

Point* Cluster::getElementList(int index){
    auto it = this->points.begin();

    advance(it,index);
    return *it;
}

Cluster* Cluster::getThCluster(int index){
    auto it = clusters.begin();
    advance(it,index);
    return *it;
}

int Cluster::getNumberCluster(){
    return Cluster::numberCluster;
}

int Cluster::getNumberElements(){
    return points_number_;
}

double Cluster::getSumCluster(int index){
    return sumCluster[index];
}

double Cluster::getSumDistance(){
    return Cluster::sumDistance;
}

void Cluster::setNumberElements(int num){
    points_number_ = num;
}

void Cluster::setSumCluster(int index, double value){
    sumCluster[index] = value;
}

void Cluster::setSumDistance(double value){
    Cluster::sumDistance = value;
}

void Cluster::setThCentroid(int index, double value){
    centroid->setThValue(index,value);
}

void Cluster::setEmptyCluster(){
    points.clear();

    points_number_ = 0;
    // Sum Cluster
    for(int i=0;i<centroid->getDim();i++){
        sumCluster[i] = 0;
    }
    // Sum Distance
    Cluster::sumDistance = 0;
}

// ------ METHODS ------
void Cluster::clustersReset(){
    for(int i=0;i<numberCluster;i++){
        Cluster::getThCluster(i)->setEmptyCluster();
    }
}

void Cluster::createKclusters(int K, int centroidDimension){
    for(int i = 0; i < K; i++){
        new Cluster(centroidDimension);
    }
}

void Cluster::createCentroid(int centroidDimension){
    centroid = new Centroid(centroidDimension);
    // Choose the value of the centroid among the points in the dataset
    int index = rand() % (Point::getNumberPoints() - 1);
    // Setta quindi il centroid con le coordinate del punto scelto random
    for(int j=0; j < centroidDimension; j++){
        centroid->setThValue(j, Point::getThPoint(index)->getThValue(j));
    }
}

void Cluster::addElement(Point *t){
    points.push_back(t);
    points_number_++;
}

void Cluster::sumPoints(){
    int centroid_dim_ = Cluster::getThCluster(0)->getCentroid()->getDim();
    for(int i = 0; i < centroid_dim_; i++){ sumCluster[i] = 0;}

    for(int i = 0; i < points_number_; i++){
        for(int j = 0; j < centroid_dim_; j++){
            sumCluster[j] += getThPoint(i)->getThValue(j);
        }
    }
}

void Cluster::sumPointsClusters(){
    int cluster_number_ = Cluster::getNumberCluster();
    for(int i = 0; i < cluster_number_; i++){
        Cluster::getThCluster(i)->sumPoints();
    }
}

double Cluster::totalMSE(){
    return Cluster::sumDistance/Point::getNumberPoints();
}

// ----- ASSIGNMENT -----
void Cluster::pointAssignment(int startIndex, int endIndex) {
    int distanzaMinimaIndex, cluster_number_ = Cluster::getNumberCluster();
    double distanzaMinima, next, d[cluster_number_]; // Buffer per memorizzare le distanze tra il punto e i centroidi

    for (int i = startIndex; i < endIndex; i++) {
        // Inizializziamo con il primo cluster
        distanzaMinimaIndex = 0;
        distanzaMinima = Point::getThPoint(i)->distanza(*Cluster::getThCluster(0)->getCentroid());
        d[0] = distanzaMinima; // Memorizziamo la distanza tra il punto e il primo centroid

        // Ciclo sui cluster restanti
        for (int j = 1; j < cluster_number_; j++) {
            next = Point::getThPoint(i)->distanza(*Cluster::getThCluster(j)->getCentroid());
            d[j] = next; // Memorizziamo la distanza tra il punto e il centroid corrente

            // Se troviamo una distanza più piccola, aggiorniamo
            if (next < distanzaMinima) {
                distanzaMinima = next;
                distanzaMinimaIndex = j;
            }
        }

        // Aggiungiamo il punto al cluster più vicino
        Cluster::getThCluster(distanzaMinimaIndex)->addElement(Point::getThPoint(i));

        // Aggiorniamo la somma delle distanze (utile per analisi successive)
        Cluster::sumDistance += d[distanzaMinimaIndex];
    }
}

void Cluster::centroidParallelCalculator(){
    int centroid_dim = centroid->getDim();

    if(points_number_){
        for(int i = 0; i < centroid_dim; i++){
            centroid->setThValue(i, sumCluster[i]/points_number_);
        }
    }else{
        for(int i = 0; i < centroid_dim; i++) {
            centroid->setThValue(i,0);
        }
    }
}

void Cluster::centroidsParallelAssignment(){
    for(int i = 0;i < Cluster::getNumberCluster(); i++){
        Cluster::getThCluster(i)->centroidParallelCalculator();
    }
}

// ----- SERIALIZE FUNCTIONS -----
void Cluster::serializeCluster(double* buffer, int k, int dim){
    buffer[0] = k;      // Number of klusters
    buffer[1] = dim;    // Centroid dimension

    for(int i = 0; i < k; i++) {
        for(int j = 0; j < dim; j++){
            buffer[2 + i * dim + j] = Cluster::getThCluster(i)->getCentroid()->getThValue(j);
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
    int K = Cluster::getNumberCluster();
    for(int i = 0; i < K; i++){
        int cluster_dim = Cluster::getThCluster(i)->getCentroid()->getDim();

        for(int j = 0; j < cluster_dim; j++){
            buffer[i * cluster_dim + j] = Cluster::getThCluster(i)->getCentroid()->getThValue(j);
        }
    }
}

void Cluster::deSerializeCentroids(double* buffer) {
    for(int i = 0; i < Cluster::getNumberCluster(); i++){
        int cluster_dim = Cluster::getThCluster(i)->getCentroid()->getDim();

        for(int j = 0; j < cluster_dim; j++){
            Cluster::getThCluster(i)->getCentroid()->setThValue(j,buffer[i * cluster_dim + j]);
        }
    }
}

void Cluster::serializeSumClusters(double* buffer){
    for(int i = 0; i < Cluster::getNumberCluster(); i++){
        int centroid_dim_ = Cluster::getThCluster(i)->getCentroid()->getDim();

        for(int j = 0; j < centroid_dim_; j++){
            buffer[i * (centroid_dim_ + 1) + j] = Cluster::getThCluster(i)->getSumCluster(j);
        }

        buffer[i * (centroid_dim_ + 1) + centroid_dim_] = Cluster::getThCluster(i)->getNumberElements();
    }
}

// [INFO] Non c'è la funzione deSerializeSumClusters

// ---- DEBUG ----
void Cluster::printClusters(){
    ofstream f;
    f.open("test_kmeans.txt");

    for(int i = 0; i < numberCluster; i++) {
        cout << "CLUSTER <"<<i<<"> ELEMENTS NUMBER = " << Cluster::getThCluster(i)->getNumberElements() << endl;
        cout << "CENTROID @ " << Cluster::getThCluster(i)->getCentroid()->toString() << endl;

        for(int j = 0; j < Cluster::getThCluster(i)->getNumberElements(); j++){
            cout << Cluster::getThCluster(i)->getElementList(j)->toString() << endl;

            f << i << ";" << Cluster::getThCluster(i)->getElementList(j)->toString() << endl;
        }
    }
    cout << "---------------------" << endl;

    f.close();
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