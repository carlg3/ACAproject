#ifndef PARALLEL_CLUSTER_H
#define PARALLEL_CLUSTER_H

#include <string>
#include <list>

#include "Tupla.h"
#include "Centroid.h"
#include "Point.h"

class Cluster{
public:
    Cluster(int centroidDimension);
    static void createKclusters(int K,int centroidDimension);
    void createCentroid(int centroidDimension);
    void setEmptyCluster(); // Reset of the cluster
    static void clustersReset(); // Reset of all the clusters
    void addElement(Point *t); // Add a point to the cluster
    static void pointAssignment(); // Assign a point to the closest cluster
    static void pointAssignment(int startIndex, int endIndex); // Assign a subset of points to the closest cluster
    void sumPoints(); // Calculate the sum of all points in the cluster
    static void sumPointsClusters(); // Calculate the sum of all points in all clusters
    void centroidParallelCalculator(); // Calculate the new centroid of the cluster
    static void centroidsParallelAssignment(); // Calculate the new centroid of all clusters
    static double totalMSE();

    // Serialization functions
    static void serializeCluster(double* buffer, int k, int dim);
    static void deserializeCluster(double* buffer);
    static void serializeCentroids(double* buffer);
    static void deSerializeCentroids(double* buffer);
    static void serializeSumClusters(double* buffer);

    static void printClusters(); // Print all the clusters
    static void printCentroids(); // Print all the centroids
    void printSum();

    // Getters and Setters
    static Cluster* getThCluster(int index);
    static int getNumberCluster();
    Centroid* getCentroid();
    int getNumberElements();
    Point* getThPoint(int index);
    double getSumCluster(int index);
    void setNumberElements(int num);
    void setSumCluster(int index, double value);
    static double getSumDistance();
    static void setSumDistance(double value);

    // Debug functions
    void setThCentroid(int index, double value);

private:
    static int numberCluster;
    static std::list<Cluster*> clusters;
    Centroid *centroid;
    int numberElements;
    std::list<Point*> points;
    double* sumCluster;
    static double sumDistance;
};

#endif //PARALLEL_CLUSTER_H
