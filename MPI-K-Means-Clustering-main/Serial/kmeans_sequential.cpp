//
// Created by galan on 08/09/2024.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include <ctime>
#include <random>

#include "Cluster.h"
#include "Point.h"

#include <chrono>
using namespace chrono;
using namespace std;

int MAX_ITERATION = 5;

// string path_gcloud = "/home/galan/ACAproject/MPI-K-Means-Clustering-main/";
string path_gcloud = "/mnt/c/Users/galan/Documents/GitHub/ACAproject/MPI-K-Means-Clustering-main/";

string dataset = path_gcloud + "DataSet/DataSet1000x2.txt";

void readDataSet(vector<Point*>& points, const string& filePath){
    ifstream dataSet(filePath);

    if (!dataSet.is_open()){
        cout << "[ERR] CAN'T OPEN DATASET FILE" << endl;
        return;
    }

    string line;
    while (getline(dataSet, line)) {
        vector<string> tokens;
        stringstream ss(line);
        string token;

        while (getline(ss, token, ',')) {
            tokens.push_back(token);
        }

        Point *point = new Point(tokens.size());
        for (int i = 0; i < (int)tokens.size(); i++) {
            point->set_value(i, stod(tokens[i]));
        }

        points.push_back(point);
    }

    dataSet.close();
}

int main(int argc, char* argv[]) {
    vector<Point*> points_temp_;

	// Reading dataset..
	if(argc == 2){
		dataset = path_gcloud + "DataSet/" + argv[1];
	}
    readDataSet(points_temp_, dataset);

    // [DEBUG]
    /*
    ofstream f("dataset_usato.txt");
    for (int i = 0; i < Point::get_spoints_(); i++) {
        f << Point::get_point(i)->toString() << endl;
    }

    f.close();
    */

    // After reading the dataset..
    auto start = high_resolution_clock::now(); // Inizia il timer
   
    int pointDimension = points_temp_[0]->get_dim();  // Dimensione del dato R^pointDimension
    int totalNumberPoint = (int)points_temp_.size();      // Numero di dati nel nostro DataSet

    // Derive cluster number..
	int K = sqrt(totalNumberPoint / 2);

    // printf("K:%d\npointDimension:%d\ntotalNumberPoint:%d\n-----", K, pointDimension, totalNumberPoint);

    Cluster::create_clusters(K, pointDimension);

    Cluster::saveCentroids(0, 88);
    Cluster::saveClusters(0, 89);

	// Setting tmse
    double previousTMSE = 0, tmse = 0;

    while ((MAX_ITERATION-- && tmse < previousTMSE) || previousTMSE == 0) {
        Cluster::reset_clusters();

        Cluster::saveCentroids(0, 97);

		// Assign points to clusters
        Cluster::map_point_to_cluster();

		// Derive centroids for each cluster
        Cluster::find_centroid_clusters();

        Cluster::saveCentroids(0, 105);

        previousTMSE = tmse;
        tmse = Cluster::totalMSE();
    }

    // Using chrono to get time spent..
    auto end = high_resolution_clock::now(); 
    auto duration = duration_cast<microseconds>(end - start).count(); // Tempo in microsecondi

    // [DEBUG] Per salvare i Cluster su file
    Cluster::saveCentroids(0, 109);
    Cluster::saveClusters(0, 110);

    cout << "That took: " << duration*1e-6 << " sec" << endl;

    return 0;
}