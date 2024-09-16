#include <list>
#include <iostream>
#include <fstream>

#include "Tupla.h"
#include "Cluster.h"

using namespace std;

double Cluster::sumDistance;
vector<Cluster*> Cluster::clusters;

// Crea un cluster e gli assegna un centroid
Cluster::Cluster(int centroidDimension){
    clusters.push_back(this);

    createCentroid(centroidDimension);

    Cluster::sumDistance = 0;

    sumCluster = new double[centroidDimension];
    initSumCluster();

    points_number_ = 0;
}

// ---- GETTERS & SETTERS ----
Centroid* Cluster::getCentroid(){
    return centroid;
}

Point* Cluster::getThPoint(int index) {
    auto it = points_.begin();
    advance(it, index);
    return *it;
}

Point* Cluster::getElementList(int index){
    auto it = points_.begin();

    advance(it,index);
    return *it;
}

Cluster* Cluster::getThCluster(int index){
    auto it = clusters.begin();
    advance(it, index);
    return *it;
}

int Cluster::getNumberCluster(){
    return clusters.size();
}

int Cluster::getNumberElements(){
    // return points_.size();
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

/*
void Cluster::setThCentroid(int index, double value){
    centroid->setThValue(index,value);
}
*/

void Cluster::setEmptyCluster(){
    points_.clear();

    initSumCluster();
    Cluster::sumDistance = 0;

    points_number_ = 0;
}

// ------ METHODS ------
void Cluster::clustersReset(){
    for(int i = 0; i < Cluster::getNumberCluster(); i++){
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
    int index = rand() % (Point::getTotalNumberPoints() - 1);
    // Setta quindi il centroid con le coordinate del punto scelto random
    for(int j=0; j < centroidDimension; j++){
        centroid->setThValue(j, Point::getThPoint(index)->getThValue(j));
    }
}

void Cluster::addElement(Point *t){
    points_.push_back(t);

    points_number_++;
}

void Cluster::initSumCluster() {
    for(int i = 0; i < centroid->getDim(); i++){ sumCluster[i] = 0;}
}

void Cluster::sumPoints(){
    initSumCluster();

    int centroid_dim_ = this->getCentroid()->getDim();
    for(int i = 0; i < Cluster::getNumberElements(); i++){
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
    return Cluster::sumDistance/Point::getTotalNumberPoints();
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
    int centroid_dim = this->centroid->getDim();
    int number_elements = this->getNumberElements();

    if(number_elements){
        for(int i = 0; i < centroid_dim; i++){
            this->centroid->setThValue(i, sumCluster[i]/number_elements);
        }
    }else{
        for(int i = 0; i < centroid_dim; i++) {
            this->centroid->setThValue(i,0);
        }
    }
}

void Cluster::centroidsParallelAssignment(){
    for(int i = 0; i < Cluster::getNumberCluster(); i++){
        Cluster::getThCluster(i)->centroidParallelCalculator();
    }
}

// ----- SERIALIZE FUNCTIONS -----
void Cluster::serializeCluster(double* buffer){
    int dim = Cluster::getThCluster(0)->getCentroid()->getDim();

    buffer[0] = clusters.size(); // Number of klusters
    buffer[1] = dim; // Centroid dimension

    for(int i = 0; i < clusters.size(); i++) {
        for(int j = 0; j < dim; j++){
            buffer[2 + i * dim + j] = Cluster::getThCluster(i)->getCentroid()->getThValue(j);
        }
    }
}

void Cluster::deserializeCluster(double* buffer){
    int K = buffer[0];
    int dim = buffer[1]; // Cluster dimension
    Cluster::createKclusters(K,dim);

    for(int i = 0; i < buffer[0]; i++) {
        for(int j = 0; j < dim; j++){
            Cluster::getThCluster(i)->getCentroid()->setThValue(j,buffer[i * dim + j + 2]);
        }
    }
}

void Cluster::serializeCentroids(double* buffer){
    int K = Cluster::getNumberCluster();
    for(int i = 0; i < K; i++){
        int dim = Cluster::getThCluster(i)->getCentroid()->getDim();

        for(int j = 0; j < dim; j++){
            buffer[i * dim + j] = Cluster::getThCluster(i)->getCentroid()->getThValue(j);
        }
    }
}

void Cluster::deserializeCentroids(double* buffer) {
    int dim = Cluster::getThCluster(0)->getCentroid()->getDim();

    for(int i = 0; i < Cluster::getNumberCluster(); i++){
        for(int j = 0; j < dim; j++){
            Cluster::getThCluster(i)->getCentroid()->setThValue(j, buffer[i * dim + j]);
        }
    }
}

void Cluster::serializeSumClusters(double *buffer){
    for(int i = 0; i < Cluster::getNumberCluster(); i++){
        int centroid_dim_ = Cluster::getThCluster(i)->getCentroid()->getDim();

        for(int j = 0; j < centroid_dim_; j++){
            buffer[i * (centroid_dim_ + 1) + j] = Cluster::getThCluster(i)->getSumCluster(j);
        }

        buffer[i * (centroid_dim_ + 1) + centroid_dim_] = Cluster::getThCluster(i)->getNumberElements();
    }
}

// [INFO] Non c'è la funzione deSerializeSumClusters
void Cluster::deserializeSumClusters(double *buffer) {
    int cluster_number_ = Cluster::getNumberCluster();
    int dim = Cluster::getThCluster(0)->getCentroid()->getDim();
    int index;

    for(int i = 0; i < cluster_number_; i++) {
        index = i * (dim + 1);

        for (int j = 0; j < dim; j++) {
            Cluster::getThCluster(i)->setSumCluster(j, buffer[index + j]);
        }

        Cluster::getThCluster(i)->setNumberElements(buffer[index + dim]);
    }
}

// ---- DEBUG ----
void Cluster::printClusters(){
    ofstream debug_txt; debug_txt.open("test_kmeans.txt");

    int cluster_number_ = clusters.size();

    printf("KMeansAlg w/ %d clusters\n", cluster_number_);
    for(int i = 0; i < cluster_number_; i++) {
        printf("CLUSTER <%d>\tNumber of elements = %d\n", i, Cluster::getThCluster(i)->getNumberElements());
    }

    for(int i = 0; i < cluster_number_; i++) {
        // printf("CLUSTER <%d>\tNumber of elements = %d\n", i, Cluster::getThCluster(i)->getNumberElements());
        printf("CENTROID of %d @ %s\n", i, Cluster::getThCluster(i)->getCentroid()->toString().c_str());

        for(int j = 0; j < Cluster::getThCluster(i)->getNumberElements(); j++){
            printf("%s\n", Cluster::getThCluster(i)->getElementList(j)->toString().c_str());

            debug_txt << i << ";" << Cluster::getThCluster(i)->getElementList(j)->toString() << endl;
        }
    }
    printf("---------------------\n");

    debug_txt.close();
}

void Cluster::printCentroids(){
    int cluster_number_ = clusters.size();

    for(int i = 0; i < cluster_number_;i++) {
        printf("CENTROID = %s\nsumCluster =\n", Cluster::getThCluster(i)->getCentroid()->toString().c_str());
        Cluster::getThCluster(i)->printSum();
        printf("1nnumberElements = %d\n", Cluster::getThCluster(i)->getNumberElements());
    }
    printf("---------------------\n");
}

void Cluster::printSum(){
    string s = "<";
    for(int i = 0; i < centroid->getDim(); i++){
        if(i==0){
            s += to_string(sumCluster[i]);
        }else{
            s += "," + to_string(sumCluster[i]);
        }
    }
    s += ">";
    cout << s;
}