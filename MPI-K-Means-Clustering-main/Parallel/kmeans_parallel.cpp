#include <iostream>
#include <stdlib.h>
#include <bits/stdc++.h>

#include <mpi.h>
/*
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/set.hpp>
#include <boost/iostreams/stream_buffer.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/back_inserter.hpp>
*/
#include "Cluster.h"
#include "Point.h"

using namespace std;

int MAXITERATION = 100;
const int LENTAG = 0, STAT = 1, DATAPOINTTAG = 2, DATACLUSTERTAG = 3, DATASUMCLUSTERTAG = 4;

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

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);
    MPI_Status status;

    srand(time(0)); // Randomize initialization point

    // Get my rank
    int my_rank, commSize;
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &commSize);

    if(my_rank == 0){
        double starttime, endtime;
        starttime = MPI_Wtime();

        vector<Point*> points_temp_;
        // readDataSet(points_temp_,"/mnt/c/Users/galan/CLionProjects/Serial-proj-test/dataset/dataset_100x2.txt");
        readDataSet(points_temp_,"/home/galan/ACAproject/MPI-K-Means-Clustering-main/Parallel/dataset/dataset_100x2.txt");

        int pointDimension = points_temp_[0]->get_dim(); // Dimensione del dato R^pointDimension
        int totalNumberPoint = points_temp_.size();      // Numero di dati nel nostro DataSet

        int K = sqrt(totalNumberPoint/2);
        Cluster::createKclusters(K, pointDimension);

        int pointsXprocessor = totalNumberPoint / commSize;

        int bufferSize = 2 + pointsXprocessor * pointDimension;

        double *buffer, *buffer2;
        buffer = new double[bufferSize];

        // Sends points to each processor
        for(int i = 1; i < commSize; i++){
            MPI_Send(&bufferSize, 1, MPI_INT, i, LENTAG, MPI_COMM_WORLD);

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

        // Task
        double previousTMSE, tmse = 0;
        int finish = 0;

        while(true) {
            Cluster::clustersReset();

            endtime = MPI_Wtime(); // Stop timer
            printf("[INFO] Pre-Assignment: %f seconds\n", endtime - starttime); // Print execution time

            // Master works on the last batch of points <pointsXprocessor>
            int startIndex = (commSize - 1) * pointsXprocessor;
            int endIndex = totalNumberPoint;
            Cluster::pointAssignment(startIndex, endIndex);

            endtime = MPI_Wtime(); // Stop timer
            printf("[INFO] Post-Assignment: %f seconds\n", endtime - starttime); // Print execution time

            // Gets the sum of the points of each clusters (of the master's points)
            Cluster::sumPointsClusters();

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

            // Gets the static value sumDistance of all cluster (in the master)
            buffer2[0] = Cluster::getSumDistance();
            // 2--- REDUCE
            MPI_Reduce(buffer2, buffer, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

            Cluster::setSumDistance(buffer[0]);

            tmse = Cluster::totalMSE();

            // Sends finish condition
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

        endtime = MPI_Wtime();
        printf("That took %f seconds\n", endtime-starttime); // Print execution time

        // DEBUG
        // Cluster::printCentroids();
    }

    if(my_rank != 0){
        int bufferSize;
        double *buffer, *buffer2;

        // Gets <pointsXprocessor>
        MPI_Recv(&bufferSize, 1, MPI_INT, 0, LENTAG, MPI_COMM_WORLD, &status);

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

            // Gets the sumPoints of the points that the slave is working with
            Cluster::sumPointsClusters();

            int cluster_number_ = Cluster::getNumberCluster();
            int centroid_dim_ = Cluster::getThCluster(0)->getCentroid()->get_dim();

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

            buffer[0] = Cluster::getSumDistance();

            // 2--- REDUCE
            MPI_Reduce(buffer, buffer2, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

            // Gets finish condition from master
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
