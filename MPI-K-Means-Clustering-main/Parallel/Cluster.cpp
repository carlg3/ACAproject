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
vector<Point*> Cluster::getPoints() {
    return points_;
};

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
    advance(it, index);
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
    int index = rand() % (Point::getTotalNumberPoints() - 1);

    // Setta quindi il centroide con le coordinate del punto scelto random
    for(int j=0; j < centroidDimension; j++){
        centroid->setThValue(j, Point::getThPoint(index)->getThValue(j));
    }
}

void Cluster::addElement(Point *t){
    points_.push_back(t);

    points_number_++;
}

void Cluster::initSumCluster() {
    for(int i = 0; i < centroid->getDim(); i++) {
        sumCluster[i] = 0;
    }
}

void Cluster::sumPoints(){
    initSumCluster();

    int centroid_dim_ = centroid->getDim();
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

/*
    Ritorna la MeanSquaredError tra la somma delle distanze dei <punti> \
    dei <Cluster> con il loro corrispondente <entroide> diviso il numero totale dei punti
*/
double Cluster::totalMSE(){
    return Cluster::sumDistance/Point::getTotalNumberPoints();
}

// ----- ASSIGNMENT -----
void Cluster::pointAssignment(int startIndex, int endIndex) {
    for (int i = startIndex; i < endIndex; i++) {
        double minDistance = Point::getThPoint(i)->distanza(*clusters.front()->getCentroid());
        Cluster* closestCluster = clusters.front();

        for (auto cluster : clusters) {
            double distance = Point::getThPoint(i)->distanza(*cluster->getCentroid());
            if (distance < minDistance) {
                minDistance = distance;
                closestCluster = cluster;
            }
        }

        closestCluster->addElement(Point::getThPoint(i));
        Cluster::sumDistance += minDistance;
    }
}

void Cluster::centroidParallelCalculator(){
    int centroid_dim = centroid->getDim();
    int number_elements = getNumberElements();

    if(number_elements){
        for(int i = 0; i < centroid_dim; i++){
            centroid->setThValue(i, sumCluster[i]/number_elements);
        }
    }else{
        for(int i = 0; i < centroid_dim; i++) {
            centroid->setThValue(i,0);
        }
    }
}

void Cluster::centroidsParallelAssignment(){
    for(int i = 0; i < Cluster::getNumberCluster(); i++){
        Cluster::getThCluster(i)->centroidParallelCalculator();
    }
}

// ----- SERIALIZING FUNCTIONS -----
// Invia solo i centroidi per ogni cluster
void Cluster::serializeCentroids(double* buffer){
    int K = Cluster::getNumberCluster();
    int dim = Cluster::getThCluster(0)->getCentroid()->getDim();

    for(int i = 0; i < K; i++){
        for(int j = 0; j < dim; j++){
            buffer[i * dim + j] = Cluster::getThCluster(i)->getCentroid()->getThValue(j);
        }
    }
}

void Cluster::deserializeCentroids(double* buffer) {
    int K = Cluster::getNumberCluster();
    int dim = Cluster::getThCluster(0)->getCentroid()->getDim();

    for(int i = 0; i < K; i++){
        for(int j = 0; j < dim; j++){
            Cluster::getThCluster(i)->getCentroid()->setThValue(j, buffer[i * dim + j]);
        }
    }
}

// Invia numero di cluster e la dimensione dei punti, poi i centroidi iniziali per ogni cluster
void Cluster::serializeCluster(double* buffer){
    int K = Cluster::getNumberCluster();
    int dim = Cluster::getThCluster(0)->getCentroid()->getDim();

    buffer[0] = K; // Number of klusters
    buffer[1] = dim; // Centroid dimension

    Cluster::serializeCentroids(buffer + 2);
}

void Cluster::deserializeCluster(double* buffer) {
    int K = buffer[0];
    int dim = buffer[1]; // Cluster dimension

    Cluster::createKclusters(K,dim);

    Cluster::deserializeCentroids(buffer + 2);
}

void Cluster::serializeSumClusters(double *buffer){
    int cluster_number_ = Cluster::getNumberCluster();
    int dim = Cluster::getThCluster(0)->getCentroid()->getDim();
    int index;

    for(int i = 0; i < cluster_number_; i++){
        index = i * (dim + 1);

        for(int j = 0; j < dim; j++){
            buffer[index + j] = Cluster::getThCluster(i)->getSumCluster(j);
        }

        buffer[index + dim] = Cluster::getThCluster(i)->getNumberElements();
    }
}

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
void Cluster::printClusters(int my_rank){
    int cluster_number_ = clusters.size();

    printf("------------- RANK <%d>\nKMeansAlg w/ %d clusters\n", my_rank, cluster_number_);

    for(int i = 0; i < cluster_number_; i++) {
        printf("Cluster(%d) Centroid @ %s\n", i, Cluster::getThCluster(i)->getCentroid()->toString().c_str());

        for(int j = 0; j < Cluster::getThCluster(i)->getPoints().size(); j++){
            printf("%s\n", Cluster::getThCluster(i)->getElementList(j)->toString().c_str());
        }
    }
    printf("---------------------\n");
}

void Cluster::printCentroids(){
    int cluster_number_ = clusters.size();

    for(int i = 0; i < cluster_number_; i++) {
        printf("Cluster(%d) Centroid @ %s\t w/ SumCluster = ", i, Cluster::getThCluster(i)->getCentroid()->toString().c_str());
        Cluster::getThCluster(i)->printSum(); // Stampa somma dei punti di quel cluster
        printf("\nnumberElements = %d\n", Cluster::getThCluster(i)->getNumberElements());
    }
    printf("---------------------\n");
}

void Cluster::printSum(){
    string s = "[";
    for(int i = 0; i < centroid->getDim(); i++){
        if(i==0){
            s += to_string((int)sumCluster[i]);
        }else{
            s += ", " + to_string((int)sumCluster[i]);
        }
    }
    s += "]";
    cout << s;
}

void Cluster::saveClusters(int my_rank){
    // Creo path del file
    string file = "rank_"; file.append(std::to_string(my_rank)); file.append(".txt");

    ofstream debug_txt(file);

    int cluster_number_ = clusters.size();

    debug_txt<<"// RANK = "<<my_rank<<"\n";

    for(int i = 0; i < cluster_number_; i++) {
        // cluster, point, isCentroid
        debug_txt<<i<<';'<<Cluster::getThCluster(i)->getCentroid()->toString().c_str()<<';'<<'y'<<"\n";

        for(int j = 0; j < Cluster::getThCluster(i)->getPoints().size(); j++){
            debug_txt<<i<<';'<<Cluster::getThCluster(i)->getElementList(j)->toString().c_str()<<';'<<'n'<<"\n";
        }
    }

    debug_txt.close();
}