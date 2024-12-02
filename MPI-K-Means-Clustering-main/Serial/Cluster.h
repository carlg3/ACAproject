//
// Created by galan on 08/09/2024.
//

#ifndef CLUSTER_H
#define CLUSTER_H

#include <string>
#include <list>

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
    static Cluster* getThCluster(int index);
    static int getNumberCluster();
    Point* getPointsList(int index);
    int getNumberPoints();
    Centroid* getCentroid();
    Point* getThPoint(int index);

    // Debug functions
    void setThCentroid(int index, double value);
    static void printClusters();

private:
    static vector<Cluster*> clusters;
    Centroid *centroid;

    vector<Point*> cluster_points_;

    static double sumDistance;
};

#endif //CLUSTER_H
