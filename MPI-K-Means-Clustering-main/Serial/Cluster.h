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
    static void create_clusters(int K, int centroid_dim);
    void create_centroid(int centroid_dim);

    void empty_cluster();
    static void reset_clusters();
    void add_point(Point *t);

    static void map_point_to_cluster();
    static void find_centroid_clusters();
    void find_centroid_();
    double mean(int index);
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
    static void saveClusters(int my_rank, int bp);
    static void saveCentroids(int my_rank, int bp);

private:
    static vector<Cluster*> clusters;
    Centroid *centroid;

    vector<Point*> cluster_points_;
    static double sumDistance;
};

#endif //CLUSTER_H
