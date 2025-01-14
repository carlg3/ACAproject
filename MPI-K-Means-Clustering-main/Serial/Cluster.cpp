//
// Created by galan on 08/09/2024.
//

#include <vector>
#include <iostream>
#include <fstream>
#include <cstdlib>

#include "Tupla.h"
#include "Cluster.h"

using namespace std;

vector<Cluster*> Cluster::clusters;

Cluster::Cluster(const int centroidDimension) {
    clusters.push_back(this);
    create_centroid(centroidDimension);
}

void Cluster::empty_cluster() {
    cluster_points_.clear();
}

void Cluster::reset_clusters() {
    for (auto cluster : clusters) {
        cluster->empty_cluster();
    }
}

void Cluster::create_clusters(int K, int centroidDimension) {
    for (int i = 0; i < K; i++) {
        new Cluster(centroidDimension);
    }
}

void Cluster::create_centroid(int centroidDimension) {
    centroid = new Centroid(centroidDimension);
    int centroidIndex = rand() % (Point::get_spoints_() - 1); // Choose the value of the centroid among the points in the dataset

    for (int j = 0; j < centroidDimension; j++) {
        centroid->set_value(j, Point::get_point(centroidIndex)->get_value(j));
    }
}

Cluster* Cluster::get_cluster(const int index) {
    auto it = clusters.begin();
    advance(it, index);
    return *it;
}

int Cluster::get_sclusters_() {
    return clusters.size();
}

int Cluster::get_spoints_() {
    return cluster_points_.size();
}

void Cluster::add_point(Point* t) {
    cluster_points_.push_back(t);
}

void Cluster::map_point_to_cluster() {
    for (int i = 0; i < Point::get_spoints_(); i++) {
        double minDistance = Point::get_point(i)->distanza(*clusters.front()->get_centroid());
        Cluster* closestCluster = clusters.front();

        for (auto cluster : clusters) {
            double distance = Point::get_point(i)->distanza(*cluster->get_centroid());
            if (distance < minDistance) {
                minDistance = distance;
                closestCluster = cluster;
            }
        }

        closestCluster->add_point(Point::get_point(i));
    }
}

void Cluster::find_centroid_clusters() {
    for (auto cluster : clusters) {
        cluster->find_centroid_();
    }
}

void Cluster::find_centroid_() {
    if (cluster_points_.size()) {
        for (int i = 0; i < centroid->get_dim(); i++) {
            centroid->set_value(i, mean(i));
        }
    }
    else {
        for (int i = 0; i < centroid->get_dim(); i++) {
            centroid->set_value(i, 0);
        }
    }
}

double Cluster::mean(int index) {
    double sum = 0;
    for (auto point : cluster_points_) {
        sum += point->get_value(index);
    }
    return sum / cluster_points_.size();
}

Centroid* Cluster::get_centroid() {
    return centroid;
}

Point* Cluster::get_point(int index) {
    auto it = cluster_points_.begin();
    advance(it, index);
    return *it;
}

Point* Cluster::get_lpoints_(int index) {
    auto it = cluster_points_.begin();
    advance(it, index);
    return *it;
}

double Cluster::totalMSE() {
    double sumDistance = 0;
    for (auto cluster : clusters) {
        for (auto point : cluster->cluster_points_) {
            sumDistance += point->distanza(*cluster->get_centroid());
        }
    }
    return sumDistance / Point::get_spoints_();
}

void Cluster::set_centroid(int index, double value) {
    centroid->set_value(index, value);
}

void Cluster::printClusters() {
    ofstream f("test_kmeans.txt");

    for (int i = 0; i < get_sclusters_(); i++) {
        cout << "CLUSTER <" << i << "> ELEMENTS NUMBER = " << get_cluster(i)->get_spoints_() << endl;
        cout << "CENTROID @ " << get_cluster(i)->get_centroid()->toString() << endl;

        for (int j = 0; j < get_cluster(i)->get_spoints_(); j++) {
            cout << get_cluster(i)->get_lpoints_(j)->toString() << endl;
            f << i << ";" << get_cluster(i)->get_lpoints_(j)->toString() << endl;
        }
    }
    cout << "---------------------" << endl;

    f.close();
}
