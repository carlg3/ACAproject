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

double Cluster::sumDistance;
vector<Cluster*> Cluster::clusters;

Cluster::Cluster(const int centroid_dim) {
    clusters.push_back(this);
    create_centroid(centroid_dim);
}

void Cluster::empty_cluster() {
    cluster_points_.clear();

    sumDistance = 0;
}

void Cluster::reset_clusters() {
    for (auto cluster : clusters) {
        cluster->empty_cluster();
    }
}

void Cluster::create_clusters(int K, int centroid_dim) {
    for (int i = 0; i < K; i++) {
        new Cluster(centroid_dim);
    }
}

void Cluster::create_centroid(int centroid_dim) {
    centroid = new Centroid(centroid_dim);

    // [DEBUG] Per settare un seed fisso; testare correttezza
    srand(42);

    int centroidIndex = rand() % (Point::get_spoints_() - 1); // Choose the value of the centroid among the points in the dataset

    for (int j = 0; j < centroid_dim; j++) {
        centroid->set_value(j, Point::get_point(centroidIndex)->get_value(j));
        // avrei potuto mettere set_tupla direttamente?
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

        bool isFirst = true;
        for (auto cluster : clusters){
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

void Cluster::find_centroid_clusters() {
    for (auto cluster : clusters) {
        cluster->find_centroid_();
    }
}

void Cluster::find_centroid_() {
    int centroid_dim = centroid->get_dim();

    // printf("points_number_: %d\n", (int) cluster_points_.size());

    if (!cluster_points_.empty()) {
        for (int i = 0; i < centroid_dim; i++) {
            // TEST
            centroid->set_value(i, mean(i));
        }
    }
    else {
        for (int i = 0; i < centroid_dim; i++) {
            centroid->set_value(i, 0);
        }
    }
}

double Cluster::mean(int index) {
    double sum = 0;
    for (auto point : cluster_points_) {
        sum += point->get_value(index);
    }
    return sum / (int) cluster_points_.size();
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
    /*
    double sumDistance = 0;
    for (auto cluster : clusters) {
        for (auto point : cluster->cluster_points_) {
            sumDistance += point->distanza(*cluster->get_centroid());
        }
    }*/
    return sumDistance / Point::get_spoints_();
}

void Cluster::set_centroid(int index, double value) {
    centroid->set_value(index, value);
}

void Cluster::saveClusters(int my_rank, int bp) {
    // Creo path del file
    string file = to_string(bp) + "_clusters_rank_"; file.append(std::to_string(my_rank)); file.append(".txt");
    ofstream f(file);

    // cout << "get_sclusters_():\t"<< get_sclusters_() << '\n';

    for (int i = 0; i < Cluster::get_sclusters_(); i++) {
        /*
         *  cout << "CLUSTER <" << i << "> ELEMENTS NUMBER = " << get_cluster(i)->get_spoints_() << endl;
         *  cout << "CENTROID @ " << get_cluster(i)->get_centroid()->toString() << endl;
        */

        // cout << i << ':'<< '\t' << Cluster::get_cluster(i)->get_spoints_() << endl;

        for (int j = 0; j < Cluster::get_cluster(i)->get_spoints_(); j++) {
            f << i << ";" << Cluster::get_cluster(i)->get_lpoints_(j)->toString() << endl;
        }
    }

    // cout << "---------------------" << endl;

    f.close();
}

void Cluster::saveCentroids(int my_rank, int bp) {
    // Creo path del file a quel BreakPoint fittizio
    string file = to_string(bp) + "_centroids_rank_"; file.append(std::to_string(my_rank)); file.append(".txt");
    ofstream f(file);

    for (int i = 0; i < Cluster::get_sclusters_(); i++) {
        f << i << ";" << Cluster::get_cluster(i)->get_centroid()->toString() << endl;
    }
    // cout << "---------------------" << endl;

    f.close();
}