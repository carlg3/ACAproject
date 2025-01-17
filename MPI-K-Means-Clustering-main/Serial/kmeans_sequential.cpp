#include <stdlib.h>
#include <iostream>
#include <bits/stdc++.h>
#include <list>

#include "Tupla.h"
#include "Cluster.h"
#include "Point.h"

#include <chrono>
using namespace std;
using namespace chrono;

int K = 0; // Number of Clusters
int MAXITERATION = 3;

const char* path_win = "/mnt/c/Users/galan/Documents/GitHub/ACAproject/MPI-K-Means-Clustering-main/DataSet/DataSet1000x2.txt";
const char* path_gcloud = "/home/galan/ACAproject/MPI-K-Means-Clustering-main/DataSet/DataSet10000x10.txt";

void readDataSet(int *pointDimension,int *totalNumberPoint){
	string buffer;
	ifstream DataSet; DataSet.open(path_win);
	if(!DataSet.is_open()){
		cout << "FILE OPENING FAILED" << endl;
		return;
	}
	while (getline(DataSet, buffer)){
		vector <string> tokens;
		stringstream check1(buffer);
		string intermediate;
		while(getline(check1, intermediate, ',')){
			tokens.push_back(intermediate);
		}
		Point *point = new Point(tokens.size());
		for(int i = 0; i < (int)tokens.size(); i++){
			point->setThValue(i,stod(tokens[i]));
		}
		*pointDimension = tokens.size();
		*totalNumberPoint = *totalNumberPoint + 1;
	}
	DataSet.close();
}

int main(){
	//time_t start, end;
	//time(&start);
	
	auto start = high_resolution_clock::now(); // Inizia il timer

	int pointDimension;    // Dimensione del dato R^pointDimension
	int totalNumberPoint = 0; // Numbers of data in our DataSet

	//srand(time(0)); // Randomize initialization point

	// READING FILE
	readDataSet(&pointDimension,&totalNumberPoint);

	// [DEBUG]
    /*
    ofstream f("dataset_usato.txt");
    for (int i = 0; i < Point::getNumberPoints(); i++) {
        f << Point::getThPoint(i)->toString() << endl;
    }

    f.close();
    */

	// INIZIALIZE CLUSTERS AND CENTROIDS
	K = sqrt(totalNumberPoint/2);
	Cluster::createKclusters(K,pointDimension);

	// INITIAL TMSE
	double previousTMSE = 0;
	double tmse = 0;

    // DEBUG
	// Cluster::saveCentroids();

	while((MAXITERATION-- && tmse<previousTMSE) || previousTMSE==0) {
		Cluster::clustersReset();
		// ASSIGN POINTS TO CLUSTERS
		Cluster::pointAssignment();
		// CALCULE CENTROIDS
		Cluster::centroidsAssignment();
		previousTMSE = tmse;
		tmse = Cluster::totalMSE();
	}

	// time(&end);
	// double time_taken = double(end - start);
	// cout << "Time taken by program is : " << time_taken << " sec " << endl;

	// Usando chrono...
	auto end = high_resolution_clock::now(); // Termina il timer
	auto duration = duration_cast<microseconds>(end - start).count(); // Tempo in microsecondi

	// [DEBUG] Per salvare i Cluster su file
    Cluster::saveClusters();
    // Per salvare i centroids
    Cluster::saveCentroids();

	cout << "Time taken to run the task: " << duration*1e-6 << " sec" << endl;
	return 0;
}
