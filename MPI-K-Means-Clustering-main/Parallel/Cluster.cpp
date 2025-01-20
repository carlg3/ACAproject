#include <iostream>
#include <fstream>
#include "Tupla.h"
#include "Cluster.h"
using namespace std;

double Cluster::sumDistance;
vector<Cluster*> Cluster::clusters;

// Crea un cluster e gli assegna un centroid
Cluster::Cluster(int centroid_dim){
    clusters.push_back(this);

    create_centroid(centroid_dim);

    Cluster::sumDistance = 0;

    sumCluster = new double[centroid_dim];
    initSumCluster();

    points_number_ = 0;
}

// ---- GETTERS & SETTERS ----
vector<Point*> Cluster::get_lpoints_() {
    return points_;
};

Centroid* Cluster::get_centroid(){
    return centroid;
}

Point* Cluster::get_point(int index) {
    auto it = points_.begin();
    advance(it, index);
    return *it;
}

Point* Cluster::get_lelements_(int index){
    auto it = points_.begin();
    advance(it, index);
    return *it;
}

Cluster* Cluster::get_cluster(int index){
    auto it = clusters.begin();
    advance(it, index);
    return *it;
}

int Cluster::get_sclusters_(){
    return clusters.size();
}

int Cluster::get_selements_(){
    return points_number_;
}

double Cluster::getSumCluster(int index){
    return sumCluster[index];
}

double Cluster::getSumDistance(){
    return Cluster::sumDistance;
}

void Cluster::set_selements_(int num){
    points_number_ = num;
}

void Cluster::setSumCluster(int index, double value){
    sumCluster[index] = value;
}

void Cluster::setSumDistance(double value){
    Cluster::sumDistance = value;
}

void Cluster::empty_cluster(){
    points_.clear();
    points_number_ = 0;

    initSumCluster();
    Cluster::sumDistance = 0;
}

// ------ METHODS ------
void Cluster::reset_clusters(){
    for(int i = 0; i < Cluster::get_sclusters_(); i++){
        Cluster::get_cluster(i)->empty_cluster();
    }
}

void Cluster::create_clusters(int K, int centroidDimension){
    for(int i = 0; i < K; i++){
        new Cluster(centroidDimension);
    }
}

void Cluster::create_centroid(int centroidDimension){
    centroid = new Centroid(centroidDimension);

    // Per generare con lo stesso <seed>
    srand(42);

    int centroid_index_ = rand() % (Point::get_spoints_() - 1);

    // Setta quindi il centroide con le coordinate del punto scelto random
    for(int j = 0; j < centroidDimension; j++){
        centroid->set_value(j, Point::get_point(centroid_index_)->get_value(j));
    }
}

void Cluster::add_point(Point *t){
    points_.push_back(t);

    points_number_++;
}

void Cluster::initSumCluster() {
    int centroid_dim_ = centroid->get_dim();

    for(int i = 0; i < centroid_dim_; i++){
        sumCluster[i] = 0;
    }
}

void Cluster::sum_points_(){
    initSumCluster();

    int centroid_dim_ = centroid->get_dim();

    for(int i = 0; i < points_number_; i++){
        for(int j = 0; j < centroid_dim_; j++){
            sumCluster[j] += get_point(i)->get_value(j);
        }
    }
}

void Cluster::sum_points_clusters(){
    int cluster_number_ = Cluster::get_sclusters_();

    for(int i = 0; i < cluster_number_; i++){
        Cluster::get_cluster(i)->sum_points_();
    }
}

/*
    Ritorna la MeanSquaredError tra la somma delle distanze dei <punti> \
    dei <Cluster> con il loro corrispondente <entroide> diviso il numero totale dei punti
*/
double Cluster::totalMSE(){
    return Cluster::sumDistance/Point::get_spoints_();
}

// ----- ASSIGNMENT -----
void Cluster::map_point_to_cluster(int startIndex, int endIndex) {
    for (int i = startIndex; i < endIndex; i++) {
        double minDistance = Point::get_point(i)->distanza(*clusters.front()->get_centroid());
        Cluster* closestCluster = clusters.front();

        bool isFirst = true;
        for (auto cluster : clusters) {
            if (isFirst) {
                isFirst = false;
                continue; // Salta il primo elemento
            }

            double distance = Point::get_point(i)->distanza(*cluster->get_centroid());

            if (distance < minDistance) {
                minDistance = distance;
                closestCluster = cluster;
            }
        }

        closestCluster->add_point(Point::get_point(i));
        Cluster::sumDistance += minDistance;
    }
}

void Cluster::find_centroid_(){
    double mean;
    int centroid_dim = centroid->get_dim();

    // printf("points_number_: %d\n", points_number_);

    if(points_number_){
        for(int i = 0; i < centroid_dim; i++){
            // TEST
            mean = sumCluster[i]/points_number_;
            centroid->set_value(i, mean);
        }
    }else{
        for(int i = 0; i < centroid_dim; i++) {
            centroid->set_value(i,0);
        }
    }
}

void Cluster::find_centroid_clusters(){
    for(int i = 0; i < Cluster::get_sclusters_(); i++){
        Cluster::get_cluster(i)->find_centroid_();
    }
}

// ----- SERIALIZING FUNCTIONS -----
// Invia solo i centroidi per ogni cluster
void Cluster::serializeCentroids(double* buffer){
    int K = Cluster::get_sclusters_();
    int dim = Cluster::get_cluster(0)->get_centroid()->get_dim();

    for(int i = 0; i < K; i++){
        for(int j = 0; j < dim; j++){
            buffer[i * dim + j] = Cluster::get_cluster(i)->get_centroid()->get_value(j);
        }
    }
}

void Cluster::deserializeCentroids(double* buffer) {
    int K = Cluster::get_sclusters_();
    int dim = Cluster::get_cluster(0)->get_centroid()->get_dim();

    for(int i = 0; i < K; i++){
        for(int j = 0; j < dim; j++){
            Cluster::get_cluster(i)->get_centroid()->set_value(j, buffer[i * dim + j]);
        }
    }
}

// Invia numero di cluster e la dimensione dei punti, poi i centroidi iniziali per ogni cluster
void Cluster::serializeCluster(double* buffer){
    int K = Cluster::get_sclusters_();
    int dim = Cluster::get_cluster(0)->get_centroid()->get_dim();

    buffer[0] = K; // Number of klusters
    buffer[1] = dim; // Centroid dimension

    Cluster::serializeCentroids(buffer + 2);
}

void Cluster::deserializeCluster(double* buffer) {
    int K = buffer[0];
    int dim = buffer[1]; // Cluster dimension

    Cluster::create_clusters(K,dim);

    Cluster::deserializeCentroids(buffer + 2);
}

void Cluster::serializeSumClusters(double *buffer){
    int cluster_number_ = Cluster::get_sclusters_();
    int dim = Cluster::get_cluster(0)->get_centroid()->get_dim();
    int index;

    for(int i = 0; i < cluster_number_; i++){
        index = i * (dim + 1);

        for(int j = 0; j < dim; j++){
            buffer[index + j] = Cluster::get_cluster(i)->getSumCluster(j);
        }

        buffer[index + dim] = Cluster::get_cluster(i)->get_selements_();
    }
}

void Cluster::deserializeSumClusters(double *buffer) {
    int cluster_number_ = Cluster::get_sclusters_();
    int dim = Cluster::get_cluster(0)->get_centroid()->get_dim();
    int index;

    for(int i = 0; i < cluster_number_; i++) {
        index = i * (dim + 1);

        for (int j = 0; j < dim; j++) {
            Cluster::get_cluster(i)->setSumCluster(j, buffer[index + j]);
        }

        Cluster::get_cluster(i)->set_selements_(buffer[index + dim]);
    }
}

// ---- DEBUG ----
void Cluster::printClusters(int my_rank){
    int cluster_number_ = clusters.size();

    printf("------------- RANK <%d>\nKMeansAlg w/ %d clusters\n", my_rank, cluster_number_);

    for(int i = 0; i < cluster_number_; i++) {
        printf("Cluster(%d) Centroid @ %s\n", i, Cluster::get_cluster(i)->get_centroid()->toString().c_str());

        for(int j = 0; j < Cluster::get_cluster(i)->get_lpoints_().size(); j++){
            printf("%s\n", Cluster::get_cluster(i)->get_lelements_(j)->toString().c_str());
        }
    }
    printf("---------------------\n");
}

void Cluster::printCentroids(){
    int cluster_number_ = clusters.size();

    for(int i = 0; i < cluster_number_; i++) {
        printf("Cluster(%d) Centroid @ %s\t w/ SumCluster = ", i, Cluster::get_cluster(i)->get_centroid()->toString().c_str());
        Cluster::get_cluster(i)->printSum(); // Stampa somma dei punti di quel cluster
        printf("\nnumberElements = %d\n", Cluster::get_cluster(i)->get_selements_());
    }
    printf("---------------------\n");
}

void Cluster::printSum(){
    string s = "[";
    for(int i = 0; i < centroid->get_dim(); i++){
        if(i==0){
            s += to_string((int)sumCluster[i]);
        }else{
            s += ", " + to_string((int)sumCluster[i]);
        }
    }
    s += "]";
    cout << s;
}

void Cluster::saveClusters(int my_rank, int bp){
    // Creo path del file
    string file = to_string(bp) + "_clusters_rank_"; file.append(std::to_string(my_rank)); file.append(".txt");
    ofstream f(file);

    f<<"// RANK = "<<my_rank<<'\n';

    for(int i = 0; i < get_sclusters_(); i++) {
        // cluster, point, isCentroid
        // f << i << ';' <<  get_cluster(i)->get_centroid()->toString().c_str() << ';' << 'y' << "\n";
        for(int j = 0; j < get_cluster(i)->get_lpoints_().size(); j++){
            f << i << ';' <<  get_cluster(i)->get_lelements_(j)->toString().c_str() << "\n";
        }
    }

    f.close();
}

void Cluster::saveCentroids(int my_rank, int bp){
    // Creo path del file a quel BreakPoint fittizio
    string file = to_string(bp) + "_centroids_rank_"; file.append(std::to_string(my_rank)); file.append(".txt");
    ofstream f(file);

    // cout << "get_sclusters_():\t"<< get_sclusters_() << '\n';

    for (int i = 0; i < get_sclusters_(); i++) {
        f << i << ";" << get_cluster(i)->get_centroid()->toString() << endl;
    }
    // cout << "---------------------" << endl;

    f.close();
}