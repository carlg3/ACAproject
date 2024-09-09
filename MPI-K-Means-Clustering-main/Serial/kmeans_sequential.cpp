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

using namespace std;

int MAX_ITERATION = 5;

void readDataSet(vector<Point*>& points, const string& filePath) {
    ifstream dataSet(filePath);

    if (!dataSet.is_open()){
        cout << "ERRORE DURANTE L'APERTURA DEL FILE" << endl;
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
        for (int i = 0; i < tokens.size(); i++) {
            point->setThValue(i, stod(tokens[i]));
        }

        points.push_back(point);
    }

    dataSet.close();
}

int main() {
    time_t start, end;
    time(&start);

    vector<Point*> points_temp_;

    srand(time(0)); // Inizializzazione casuale dei punti

    // LETTURA DEL FILE
    // "/mnt/c/Users/galan/CLionProjects/Serial-proj-test/dataset/dataset_100x2.txt"
    readDataSet(points_temp_,R"(C:\Users\galan\CLionProjects\Serial-proj-test\dataset\dataset_100x2.txt)");

    int pointDimension = points_temp_[0]->getDim();  // Dimensione del dato R^pointDimension
    int totalNumberPoint = points_temp_.size();           // Numero di dati nel nostro DataSet

    // INIZIALIZZAZIONE CLUSTERS E CENTROIDI
    int K = sqrt(totalNumberPoint / 2);
    Cluster::createKclusters(K, pointDimension);

    // TMSE INIZIALE
    double previousTMSE = 0;
    double tmse = 0;

    while ((MAX_ITERATION-- && tmse < previousTMSE) || previousTMSE == 0) {
        Cluster::clustersReset();

        // ASSEGNAZIONE DEI PUNTI AI CLUSTERS
        Cluster::pointAssignment();

        // CALCOLO DEI CENTROIDI
        Cluster::centroidsAssignment();

        previousTMSE = tmse;
        tmse = Cluster::totalMSE();
    }

    time(&end);
    double time_taken = double(end - start);
    cout << "Time taken to run the task: " << time_taken << " sec" << endl;

    Cluster::printClusters();

    return 0;
}
