#ifndef ACAPROJECT_CLUSTER_H
#define ACAPROJECT_CLUSTER_H

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
    Point* getElementList(int index);
    int getNumberElements();
    Centroid* getCentroid();
    Point* getThPoint(int index);

    // Debug functions
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