#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include <ctime>

#include "Tupla.h"
#include "Cluster.h"
#include "Point.h"

using namespace std;

int K = 0; // Numero di Clusters
int MAX_ITERATION = 5;

void readDataSet(int& pointDimension, int& totalNumberPoint) {
    string buffer;
    ifstream DataSet("/mnt/c/Users/galan/Desktop/Uni/UNIPV/ACAproject/MPI-K-Means-Clustering-main/Utility/dataset_100x2.txt");

    if (!DataSet.is_open()) {
        cout << "ERRORE DURANTE L'APERTURA DEL FILE" << endl;
        return;
    }

    while (getline(DataSet, buffer)) {
        vector<string> tokens;
        stringstream check1(buffer);
        string intermediate;

        while (getline(check1, intermediate, ',')) {
            tokens.push_back(intermediate);
        }

        Point* point = new Point(tokens.size());
        for (int i = 0; i < tokens.size(); i++) {
            point->setThValue(i, stod(tokens[i]));
        }

        pointDimension = tokens.size();
        totalNumberPoint++;
    }

    DataSet.close();
}

int main() {
    time_t start, end;
    time(&start);

    int pointDimension = 0;         // Dimensione del dato R^pointDimension
    int totalNumberPoint = 0;       // Numero di dati nel nostro DataSet

    srand(time(0)); // Inizializzazione casuale dei punti

    // LETTURA DEL FILE
    readDataSet(pointDimension, totalNumberPoint);

    // INIZIALIZZAZIONE CLUSTERS E CENTROIDI
    K = sqrt(totalNumberPoint / 2);
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
    cout << "Tempo impiegato dal programma: " << time_taken << " sec" << endl;

    Cluster::printClusters();

    return 0;
}
