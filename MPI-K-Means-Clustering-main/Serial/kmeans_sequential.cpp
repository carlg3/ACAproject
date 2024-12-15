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

int MAX_ITERATION = 10;

string path_gcloud = "/home/galan/ACAproject/MPI-K-Means-Clustering-main/";
// string path_gcloud = "/mnt/c/Users/galan/Documents/GitHub/ACAproject/MPI-K-Means-Clustering-main/";

string dataset = path_gcloud + "DataSet/DataSet10000x10.txt";

void readDataSet(vector<Point*>& points, const string& filePath) {
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
            point->setThValue(i, stod(tokens[i]));
        }

        points.push_back(point);
    }

    dataSet.close();
}

int main() {
    vector<Point*> points_temp_;

	// Reading dataset..
    readDataSet(points_temp_, dataset);

    // After reading the dataset..
    auto start = high_resolution_clock::now(); // Inizia il timer
   
    int pointDimension = points_temp_[0]->getDim();  // Dimensione del dato R^pointDimension
    int totalNumberPoint = points_temp_.size();      // Numero di dati nel nostro DataSet

    // Derive cluster number..
	int K = sqrt(totalNumberPoint / 2);
    Cluster::createKclusters(K, pointDimension);

	// Setting tmse
    double previousTMSE = 0, tmse = 0;

    while ((MAX_ITERATION-- && tmse < previousTMSE) || previousTMSE == 0) {
        Cluster::clustersReset();
		
		// Assign points to clusters
        Cluster::pointAssignment();

		// Derive centroids for each cluster
        Cluster::centroidsAssignment();

        previousTMSE = tmse;
        tmse = Cluster::totalMSE();
    }

    // Using chrono to get time spent..
    auto end = high_resolution_clock::now(); 
    auto duration = duration_cast<microseconds>(end - start).count(); // Tempo in microsecondi

    cout << "That took: " << duration*1e-6 << " sec" << endl;

    return 0;
}