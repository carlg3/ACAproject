//
// Created by galan on 08/09/2024.
//

#include <list>
#include <iostream>
#include <fstream>
#include <cstdlib>

#include "Tupla.h"
#include "Cluster.h"

using namespace std;

list<Cluster*> Cluster::clusters;

Cluster::Cluster(const int centroidDimension) {
    clusters.push_back(this);
    createCentroid(centroidDimension);
}

void Cluster::setEmptyCluster() {
    cluster_points_.clear();
}

void Cluster::clustersReset() {
    for (auto cluster : clusters) {
        cluster->setEmptyCluster();
    }
}

void Cluster::createKclusters(int K, int centroidDimension) {
    for (int i = 0; i < K; i++) {
        new Cluster(centroidDimension);
    }
}

void Cluster::createCentroid(int centroidDimension) {
    centroid = new Centroid(centroidDimension);
    int centroidIndex = rand() % (Point::getNumberPoints() - 1); // Choose the value of the centroid among the points in the dataset

    for (int j = 0; j < centroidDimension; j++) {
        centroid->setThValue(j, Point::getThPoint(centroidIndex)->getThValue(j));
    }
}

Cluster* Cluster::getThCluster(const int index) {
    auto it = clusters.begin();
    advance(it, index);
    return *it;
}

int Cluster::getNumberCluster() {
    return clusters.size();
}

int Cluster::getNumberPoints() {
    return cluster_points_.size();
}

void Cluster::addElement(Point* t) {
    cluster_points_.push_back(t);
}

void Cluster::pointAssignment() {
    for (int i = 0; i < Point::getNumberPoints(); i++) {
        double minDistance = Point::getThPoint(i)->distanza(*clusters.front()->getCentroid());
        Cluster* closestCluster = clusters.front();

        for (auto cluster : clusters) {
            double distance = Point::getThPoint(i)->distanza(*cluster->getCentroid());
            if (distance < minDistance) {
                minDistance = distance;
                closestCluster = cluster;
            }
        }

        closestCluster->addElement(Point::getThPoint(i));
    }
}

void Cluster::centroidsAssignment() {
    for (auto cluster : clusters) {
        cluster->centroidCalculator();
    }
}

void Cluster::centroidCalculator() {
    if (cluster_points_.size()) {
        for (int i = 0; i < centroid->getDim(); i++) {
            centroid->setThValue(i, meanCalculator(i));
        }
    }
    else {
        for (int i = 0; i < centroid->getDim(); i++) {
            centroid->setThValue(i, 0);
        }
    }
}

double Cluster::meanCalculator(int index) {
    double sum = 0;
    for (auto point : cluster_points_) {
        sum += point->getThValue(index);
    }
    return sum / cluster_points_.size();
}

Centroid* Cluster::getCentroid() {
    return centroid;
}

Point* Cluster::getThPoint(int index) {
    auto it = cluster_points_.begin();
    advance(it, index);
    return *it;
}

Point* Cluster::getPointsList(int index) {
    auto it = cluster_points_.begin();
    advance(it, index);
    return *it;
}

double Cluster::totalMSE() {
    double sumDistance = 0;
    for (auto cluster : clusters) {
        for (auto point : cluster->cluster_points_) {
            sumDistance += point->distanza(*cluster->getCentroid());
        }
    }
    return sumDistance / Point::getNumberPoints();
}

void Cluster::setThCentroid(int index, double value) {
    centroid->setThValue(index, value);
}

void Cluster::printClusters() {
    ofstream f("test_kmeans.txt");

    for (int i = 0; i < getNumberCluster(); i++) {
        cout << "CLUSTER <" << i << "> ELEMENTS NUMBER = " << getThCluster(i)->getNumberPoints() << endl;
        cout << "CENTROID @ " << getThCluster(i)->getCentroid()->toString() << endl;

        for (int j = 0; j < getThCluster(i)->getNumberPoints(); j++) {
            cout << getThCluster(i)->getPointsList(j)->toString() << endl;
            f << i << ";" << getThCluster(i)->getPointsList(j)->toString() << endl;
        }
    }
    cout << "---------------------" << endl;

    f.close();
}
