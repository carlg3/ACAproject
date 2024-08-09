#ifndef ACAPROJECT_CLUSTER_H
#define ACAPROJECT_CLUSTER_H

#include <string>
#include <list>

#include "Tupla.h"
#include "Centroid.h"
#include "Point.h"

class Cluster{
    public:
    static void createKclusters(int K,int centroidDimension);
    static Cluster* getThCluster(int index);
    static int getNumberCluster();
    Cluster(int centroidDimension);
    void setEmptyCluster();
    void addElement(Point *t);
    void centroidCalculator();
    double meanCalculator(int index);
    Point* getElementList(int index);
    int getNumberElements();
    Centroid* getCentroid();
    Point* getThPoint(int index);
    void createCentroid(int centroidDimension);
    static void clustersReset();
    static void pointAssignment();
    static void centroidsAssignment();
    double MSECluster();
    static double totalMSE();
    static void printClusters();
    void setThCentroid(int index, double value);
    private:
    static int numberCluster;
    static std::list<Cluster*> clusters;
    Centroid *centroid;
    int numberElements;
    std::list<Point*> points;
    static double sumDistance;

};

#endif //ACAPROJECT_CLUSTER_H