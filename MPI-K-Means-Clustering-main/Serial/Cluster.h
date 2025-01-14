//
// Created by galan on 08/09/2024.
//

#ifndef CLUSTER_H
#define CLUSTER_H

#include <string>

#include "Centroid.h"
#include "Point.h"

using namespace std;

class Cluster{
public:
    Cluster(int centroidDimension);
    static void createKclusters(int K,int centroidDimension);
    void createCentroid(int centroidDimension);

    void setEmptyCluster();
    static void clustersReset();
    void addElement(Point *t);

    static void pointAssignment();
    static void centroidsAssignment();
    void centroidCalculator();
    double meanCalculator(int index);
    double MSECluster();
    static double totalMSE();

    // Getters and Setters
    static Cluster* get_cluster(int index);
    static int get_sclusters_();
    Point* get_lpoints_(int index);
    int get_spoints_();
    Centroid* get_centroid();
    Point* get_point(int index);

    // Debug functions
    void set_centroid(int index, double value);
    static void printClusters();

private:
    static vector<Cluster*> clusters;
    Centroid *centroid;

    vector<Point*> cluster_points_;

    static double sumDistance;
};

#endif //CLUSTER_H
