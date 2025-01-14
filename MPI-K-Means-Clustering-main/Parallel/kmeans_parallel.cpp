#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <bits/stdc++.h>
#include <string>

#include <mpi.h>

#include "Cluster.h"
#include "Point.h"

using namespace std;

int MAXITERATION = 5;
const int LENTAG = 0, STAT = 1, DATAPOINTTAG = 2, DATACLUSTERTAG = 3, DATASUMCLUSTERTAG = 4;

// string path_gcloud = "/home/galan/ACAproject/MPI-K-Means-Clustering-main/";
string path_gcloud = "/mnt/c/Users/galan/Documents/GitHub/ACAproject/MPI-K-Means-Clustering-main/";

string dataset = path_gcloud + "DataSet/DataSet50000x10.txt";

void writeExTime(int cs, int tnp, int pd, int K, double time){
    ofstream f;
    f.open((path_gcloud + "Parallel/execution_time.txt").c_str(), ios::app);
    f << cs << ";" << tnp << ";" << pd << ";" << K << ";" << time << endl;
    f.close();
}

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
        for (int i = 0; i < tokens.size(); i++) {
            point->setThValue(i, stod(tokens[i]));
        }

        points.push_back(point);
    }

    dataSet.close();
}

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);
    MPI_Status status;

    int my_rank, commSize;
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &commSize);

    if(my_rank == 0){
        double start_time, end_time;

        // Reading the dataset and derive the infos for running the kmeans
        vector<Point*> points_temp_;
		if(argc == 2){
			// Se ho passato un argomento
			dataset = path_gcloud + "DataSet/" + argv[1];
		}
        readDataSet(points_temp_, dataset);

        // After reading the dataset
        start_time = MPI_Wtime();

        int pointDimension = points_temp_[0]->getDim();     // Dimensione del dato R^pointDimension
        int totalNumberPoint = (int)points_temp_.size();    // Numero di dati nel nostro DataSet

        int K = sqrt(totalNumberPoint/2);
        Cluster::createKclusters(K, pointDimension);

        int pointsXprocessor = totalNumberPoint / commSize;
        int bufferSize = 2 + pointsXprocessor * pointDimension;

        double *buffer, *buffer2;
        buffer = new double[bufferSize];

        // Sends derived points to each other processor
        MPI_Bcast(&bufferSize, 1, MPI_INT, 0, MPI_COMM_WORLD);
        
        for(int i = 1; i < commSize; i++){
            int startIndex = (i - 1) * pointsXprocessor;
            int endIndex = startIndex + pointsXprocessor;
            Point::serializePoint(buffer, startIndex, endIndex, pointDimension);

            MPI_Send(buffer, bufferSize, MPI_DOUBLE, i, DATAPOINTTAG, MPI_COMM_WORLD);
        }

        delete[] buffer;
        buffer = nullptr;

        bufferSize = 2 + K * pointDimension;
        buffer = new double[bufferSize];

        MPI_Bcast(&bufferSize, 1, MPI_INT, 0, MPI_COMM_WORLD);

        // Sends number of clusters and init centroids
        Cluster::serializeCluster(buffer);
        MPI_Bcast(buffer, bufferSize, MPI_DOUBLE, 0, MPI_COMM_WORLD);

        // Start of the task
        double previousTMSE, tmse = 0;
        int finish = 0;

        while(true) {
            Cluster::clustersReset();

            // Master works on the last batch of points <pointsXprocessor>
            int startIndex = (commSize - 1) * pointsXprocessor;
            int endIndex = totalNumberPoint;
            Cluster::pointAssignment(startIndex, endIndex);

            // Gets the sum of the points of each cluster (of the master's points)
            Cluster::sumPointsClusters();

            // ..redundant..
            int cluster_number_ = K;
            int centroid_dim_ = pointDimension;

            bufferSize = cluster_number_ + cluster_number_ * centroid_dim_;

            buffer2 = new double[bufferSize];
            buffer = new double[bufferSize];

            Cluster::serializeSumClusters(buffer2);

            // 1--- REDUCE
            MPI_Reduce(buffer2, buffer, bufferSize, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

            Cluster::deserializeSumClusters(buffer);

            // Gets new centroids
            Cluster::centroidsParallelAssignment();

            // new TMSE used by the stopping criterion
            previousTMSE = tmse;

            delete[] buffer;
            buffer = nullptr;

            buffer = new double[1];     // recv
            buffer2 = new double[1];    // send

            // Gets the static value sumDistance of all cluster (of the master's points)
            buffer2[0] = Cluster::getSumDistance();

            // 2--- REDUCE
            MPI_Reduce(buffer2, buffer, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

            // Sets the sumDistance (in the master) derived by the sums of each slave's sumDistance
            Cluster::setSumDistance(buffer[0]);

            tmse = Cluster::totalMSE();

            // Check finish condition and send it
            if(!((MAXITERATION-- && tmse < previousTMSE) || previousTMSE == 0)){ break;}

            MPI_Bcast(&finish, 1, MPI_INT, 0, MPI_COMM_WORLD);

            // If not finished, calculates new centroids
            bufferSize = cluster_number_ * centroid_dim_;
            buffer = new double[bufferSize];

            Cluster::serializeCentroids(buffer);
            
            MPI_Bcast(&bufferSize, 1, MPI_INT, 0, MPI_COMM_WORLD);
            MPI_Bcast(buffer, bufferSize, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        }

        finish = 1;
        MPI_Bcast(&finish, 1, MPI_INT, 0, MPI_COMM_WORLD);

        end_time = MPI_Wtime();
        printf("That took %f seconds\n", end_time - start_time); // Print execution time

        // DEBUG -- CLion
        // int debug = 0;
        // while(debug == 0) {
        //     sleep(1);
        // }

        // DEBUG -- per salvare il tempo che ci si mette ad ogni esecuzione [MASTER]
        // <processi-- commSize> <numero di punti-- totalNumberPoint> <dimensione punti-- pointDimension> <numero di cluster> <tempo di esecuzione>
        writeExTime(commSize, totalNumberPoint, pointDimension, K, end_time - start_time);
    }

    if(my_rank != 0){
        int bufferSize;
        double *buffer, *buffer2;

        // Gets <pointsXprocessor>
        MPI_Bcast(&bufferSize, 1, MPI_INT, 0, MPI_COMM_WORLD);

        buffer = new double[bufferSize];
        MPI_Recv(buffer, bufferSize, MPI_DOUBLE, 0, DATAPOINTTAG, MPI_COMM_WORLD, &status);

        Point::deserializePoint(buffer);

        // Gets number of clusters and centroids
        MPI_Bcast(&bufferSize, 1, MPI_INT, 0, MPI_COMM_WORLD);

        delete[] buffer;
        buffer = nullptr;

        buffer = new double[bufferSize];

        MPI_Bcast(buffer, bufferSize, MPI_DOUBLE, 0, MPI_COMM_WORLD);

        Cluster::deserializeCluster(buffer);

        int finish;

        delete[] buffer;
        buffer = nullptr;

        while(true) {
            Cluster::clustersReset();

            // Assign points to clusters with the nearest centroid
            Cluster::pointAssignment();

            // Calculates the sumPoints of the points that the slave is working with
            Cluster::sumPointsClusters();

            int cluster_number_ = Cluster::getNumberCluster();
            int centroid_dim_ = Cluster::getThCluster(0)->getCentroid()->getDim();

            bufferSize = cluster_number_ + cluster_number_ * centroid_dim_;

            buffer = new double[bufferSize]; // send
            buffer2 = new double[bufferSize];
            Cluster::serializeSumClusters(buffer);

            // 1--- REDUCE
            MPI_Reduce(buffer, buffer2, bufferSize, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

            // CALCULATE MSE
            delete[] buffer;
            delete[] buffer2;
            buffer = nullptr;
            buffer2 = nullptr;

            buffer = new double[1];
            buffer2 = new double[1];

            // Get the sumDistance - calculated after the pointAssignment - of the clusters
            buffer[0] = Cluster::getSumDistance();

            // 2--- REDUCE | Sends the master its sumDistance
            MPI_Reduce(buffer, buffer2, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

            // Get finish condition from master
            MPI_Bcast(&finish, 1, MPI_INT, 0, MPI_COMM_WORLD);
            if(finish){ break;}

            // If not finished, waits for new cluster centroids
            MPI_Bcast(&bufferSize, 1, MPI_INT, 0, MPI_COMM_WORLD);

            delete[] buffer;
            buffer = nullptr;

            buffer = new double[bufferSize];
            MPI_Bcast(buffer, bufferSize, MPI_DOUBLE, 0, MPI_COMM_WORLD);

            Cluster::deserializeCentroids(buffer);
        }
    }

    MPI_Finalize();

    return 0;
}
