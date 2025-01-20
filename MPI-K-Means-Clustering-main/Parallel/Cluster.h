#ifndef PARALLEL_CLUSTER_H
#define PARALLEL_CLUSTER_H

#include "Tupla.h"
#include "Centroid.h"
#include "Point.h"
using namespace std;

class Cluster{
public:
    Cluster(int centroid_dim);

    // Getters and Setters
    static Cluster* get_cluster(int index);
    static int get_sclusters_();
	Point* get_lelements_(int index);
	int get_selements_();
    Centroid* get_centroid();
    Point* get_point(int index);
    vector<Point*> get_lpoints_();

    double getSumCluster(int index);
    void set_selements_(int num);
    void setSumCluster(int index, double value);
    static double getSumDistance();
    static void setSumDistance(double value);

    // Methods
    static void create_clusters(int K,int centroid_dim);
    void create_centroid(int centroid_dim);

    void empty_cluster(); // Reset of the cluster
    static void reset_clusters(); // Reset of all the clusters
    void initSumCluster();
    void add_point(Point *t); // Add a point to the cluster

    static void map_point_to_cluster(int startIndex = 0, int endIndex = Point::get_spoints_()); // Assign a subset of points to the closest cluster
    void sum_points_(); // Calculate the sum of all points in the cluster
    static void sum_points_clusters(); // Calculate the sum of all points in all clusters
    void find_centroid_(); // Calculate the new centroid of the cluster
    static void find_centroid_clusters(); // Calculate the new centroid of all clusters
    static double totalMSE();

    // Debug functions
    void printSum();
    static void printCentroids(); // Print all the centroids
    static void printClusters(int my_rank); // Print all the clusters
    static void saveClusters(int my_rank, int bp); // Saves the clusters and their points in a .txt
    static void saveCentroids(int my_rank, int bp); // Saves the centroids in a .txt
    
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
    double *sumCluster; // di _questo_ cluster rispetto la dim
};

#endif //PARALLEL_CLUSTER_H