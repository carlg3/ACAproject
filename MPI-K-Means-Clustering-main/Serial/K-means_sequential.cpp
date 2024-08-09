#include <stdlib.h>
#include <iostream>
#include <bits/stdc++.h>
#include <list>

#include "Tupla.h"
#include "Cluster.h"
#include "Point.h"

using namespace std;

int K = 0; // Number of Clusters
int MAXITERATION = 5;

void readDataSet(int *pointDimension,int *totalNumberPoint){
    string buffer;
    ifstream DataSet; DataSet.open("/mnt/c/Users/Cristian/IdeaProjects/DatasetGenerator/src/DataSet10000x10.txt");
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
        for(int i = 0; i < tokens.size(); i++){
            point->setThValue(i,stod(tokens[i]));
        }
        *pointDimension = tokens.size();
        *totalNumberPoint = *totalNumberPoint + 1;
    }
    DataSet.close();
}

int main(){
    time_t start, end;
    time(&start);
    int pointDimension;    // Dimensione del dato R^pointDimension
    int totalNumberPoint = 0; // Numbers of data in our DataSet

    srand(time(0)); // Randomize initialization point

    // READING FILE
    readDataSet(&pointDimension,&totalNumberPoint);

    // INIZIALIZE CLUSTERS AND CENTROIDS
    K = sqrt(totalNumberPoint/2);
    Cluster::createKclusters(K,pointDimension);
    // INITIAL TMSE
    double previousTMSE = 0;
    double tmse = 0;
    //Cluster::printClusters();

    while((MAXITERATION-- && tmse<previousTMSE) || previousTMSE==0) {
        Cluster::clustersReset();
        // ASSIGN POINTS TO CLUSTERS
        Cluster::pointAssignment();
        // CALCULE CENTROIDS
        Cluster::centroidsAssignment();
        previousTMSE = tmse;
        tmse = Cluster::totalMSE();
    }

    time(&end);
    double time_taken = double(end - start);
    cout << "Time taken by program is : " << time_taken << " sec " << endl;

    return 0;
}