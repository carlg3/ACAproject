#ifndef PARALLEL_CLUSTER_H
#define PARALLEL_CLUSTER_H

#include <string>
#include <list>

#include "Tupla.h"
#include "Centroid.h"
#include "Point.h"
using namespace std;

class Cluster{
public:
    Cluster(int centroidDimension);

    // Getters and Setters
    static Cluster* getThCluster(int index);
    static int getNumberCluster();
	Point* getElementList(int index);
	int getNumberElements();	
    Centroid* getCentroid();
    Point* getThPoint(int index);

    double getSumCluster(int index);
    void setNumberElements(int num);
    void setSumCluster(int index, double value);
    static double getSumDistance();
    static void setSumDistance(double value);

    // Methods
    static void createKclusters(int K,int centroidDimension);
    void createCentroid(int centroidDimension);

    void setEmptyCluster(); // Reset of the cluster
    static void clustersReset(); // Reset of all the clusters
    void initSumCluster();
    void addElement(Point *t); // Add a point to the cluster

    // static void pointAssignment(); // Assign a point to the closest cluster
    static void pointAssignment(int startIndex = 0, int endIndex = Point::getTotalNumberPoints()); // Assign a subset of points to the closest cluster
    void sumPoints(); // Calculate the sum of all points in the cluster
    static void sumPointsClusters(); // Calculate the sum of all points in all clusters
    void centroidParallelCalculator(); // Calculate the new centroid of the cluster
    static void centroidsParallelAssignment(); // Calculate the new centroid of all clusters
    static double totalMSE();

    // Debug functions
    // void setThCentroid(int index, double value);

	static void printClusters(); // Print all the clusters
    static void printCentroids(); // Print all the centroids
    void printSum();
    
    // Serialization functions
    static void serializeCluster(double* buffer);
    static void deserializeCluster(double* buffer);
    static void serializeCentroids(double* buffer);
    static void deserializeCentroids(double* buffer);
    static void serializeSumClusters(double* buffer);
    static void deserializeSumClusters(double* buffer);

private:
    static vector<Cluster*> clusters;
    static double sumDistance; // delle distanze di _tutti_ i cluster

    int points_number_;
    vector<Point*> points_; // di _questo_ cluster
    Centroid *centroid;
    double *sumCluster;
};

#endif //PARALLEL_CLUSTER_H
