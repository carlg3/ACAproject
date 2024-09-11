#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include <ctime>
#include <mpi.h>

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/set.hpp>
#include <boost/iostreams/stream_buffer.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/back_inserter.hpp>

#include "Tupla.h"
#include "Cluster.h"
#include "Point.h"

using namespace std;

int K = 0, MAXITERATION = 100;
const int LEN_TAG = 0, STAT = 1, DATA_POINT_TAG = 2, DATA_CLUSTER_TAG = 3, DATA_SUM_CLUSTER_TAG = 4;

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

int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);
    MPI_Status status;

    srand(time(0)); // Randomize initialization point

    // Get my rank and commSize
    int my_rank, commSize;
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &commSize);

    // Master
    if (my_rank == 0) {
        double starttime, endtime; // Time variables
        starttime = MPI_Wtime();   // Start timer

        vector<Point*> points_temp_;

        // LETTURA DEL FILE
        // "/mnt/c/Users/galan/CLionProjects/Serial-proj-test/dataset/dataset_100x2.txt"
        //  readDataSet(points_temp_,R"(C:\Users\galan\CLionProjects\Serial-proj-test\dataset\dataset_100x2.txt)");
        readDataSet(points_temp_,"/mnt/c/Users/galan/CLionProjects/Serial-proj-test/dataset/dataset_100x2.txt");

        int pointDimension = points_temp_[0]->getDim();  // Dimensione del dato R^pointDimension
        int totalNumberPoint = points_temp_.size();      // Numero di dati nel nostro DataSet

        // INIT CLUSTERS AND CENTROIDS
        K = sqrt(totalNumberPoint / 2);
        Cluster::createKclusters(K, pointDimension);
        //-------------------------------------------------------------------
        double *clusters_buff, *buffer2;

        int pointsXprocessor = totalNumberPoint / commSize;

        // SERIALIZING AND SENDING POINTS
        int bufferSize = 2 + pointsXprocessor * pointDimension;
        clusters_buff = new double[bufferSize];

        // Send length, then data
        for (int i = 1; i < commSize; i++) {
            MPI_Send(&bufferSize, 1, MPI_INT, i, LEN_TAG, MPI_COMM_WORLD);

            int s_i = (i - 1) * pointsXprocessor;
            int e_i = s_i + pointsXprocessor;

            Point::serializePoint(clusters_buff, s_i, e_i, pointDimension);
            MPI_Send(clusters_buff, bufferSize, MPI_DOUBLE, i, DATA_POINT_TAG, MPI_COMM_WORLD);
        }

        // SEND CLUSTERS
        bufferSize = 2 + K * pointDimension;
        clusters_buff = new double[bufferSize];

        MPI_Bcast(&bufferSize, 1, MPI_INT, 0, MPI_COMM_WORLD);
        Cluster::serializeCluster(clusters_buff, K, pointDimension);
        MPI_Bcast(clusters_buff, bufferSize, MPI_DOUBLE, 0, MPI_COMM_WORLD);

        list<Point*> points;
        int finish = 0;

        // INITIAL TMSE
        double previousTMSE = 0, tmse = 0;

        while (true) {
            Cluster::clustersReset();

            // ASSIGN POINTS TO CLUSTERS WITH NEAREST CENTROID
            endtime = MPI_Wtime(); // Stop timer
            printf("PRE POINT ASSIGNMENT %f seconds\n", endtime - starttime); // Print execution time

            int s_i = (commSize - 1) * pointsXprocessor;
            int e_i = totalNumberPoint;
            Cluster::pointAssignment(s_i, e_i);

            endtime = MPI_Wtime(); // Stop timer
            printf("AFTER POINT ASSIGNMENT %f seconds\n", endtime - starttime); // Print execution time

            // + CALCULATE MASTER SUM_CLUSTER
            for (int i = 0; i < Cluster::getNumberCluster(); i++) {
                Cluster::sumPointsClusters();
            }

            // + RECV SUM_CLUSTER AND NUMBER_OF_POINTS, AND CALCULATE TOTAL_SUM_CLUSTER AND TOTAL_NUMBER_OF_POINTS
            int points_dim = Cluster::getThCluster(0)->getCentroid()->getDim();
            int cluster_size_ = Cluster::getNumberCluster();
            bufferSize = points_dim * cluster_size_ + cluster_size_;

            clusters_buff = new double[bufferSize];    // buffer recv
            buffer2 = new double[bufferSize];   // buffer send, containing master data

            Cluster::serializeSumClusters(buffer2);

            // Calcola tutto
            MPI_Reduce(buffer2, clusters_buff, bufferSize, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

            for (int i = 0; i < Cluster::getNumberCluster(); i++) {
                int cluster_dim = Cluster::getThCluster(i)->getCentroid()->getDim();

                for (int j = 0; j < cluster_dim; j++) {
                    Cluster::getThCluster(i)->setSumCluster(j, i * (cluster_dim + 1) + j);
                }

                Cluster::getThCluster(i)->setNumberElements(clusters_buff[i * (cluster_dim + 1) + cluster_dim]);
            }

            // CALCULATE NEW CENTROIDS
            Cluster::centroidsParallelAssignment();

            // CALCULATE TMSE

            // Recv SumDistance
            previousTMSE = tmse;
            clusters_buff = new double[1];      // buffer recv
            buffer2 = new double[1];            // buffer send
            buffer2[0] = Cluster::getSumDistance();

            MPI_Reduce(buffer2, clusters_buff, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
            Cluster::setSumDistance(clusters_buff[0]);
            tmse = Cluster::totalMSE();

            // SEND FINISH CONDITION
            if (!((MAXITERATION-- && tmse < previousTMSE) || previousTMSE == 0)) { break;}
            // -------

            MPI_Bcast(&finish, 1, MPI_INT, 0, MPI_COMM_WORLD);

            // SEND NEW CENTROIDS
            bufferSize = Cluster::getNumberCluster() * pointDimension;
            clusters_buff = new double[bufferSize];

            Cluster::serializeCentroids(clusters_buff);
            MPI_Bcast(&bufferSize, 1, MPI_INT, 0, MPI_COMM_WORLD);
            MPI_Bcast(clusters_buff, bufferSize, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        }

        finish = 1;
        MPI_Bcast(&finish, 1, MPI_INT, 0, MPI_COMM_WORLD);

        endtime = MPI_Wtime(); // Stop timer
        printf("That took %f seconds\n", endtime - starttime); // Print execution time

        // Debug
        Cluster::printClusters();
    }

    // Slave
    if (my_rank != 0) {
        int finish;
        int bufferSize = 0;
        double *buff, *buffer2;

        // RECV POINTS (Recv length, then data)
        MPI_Recv(&bufferSize, 1, MPI_INT, 0, LEN_TAG, MPI_COMM_WORLD, &status);

        buff = new double[bufferSize];
        MPI_Recv(buff, bufferSize, MPI_DOUBLE, 0, DATA_POINT_TAG, MPI_COMM_WORLD, &status);

        // ++ DESERIALIZATION points recvd
        Point::deserializePoint(buff);

        // RECV CLUSTERS (Recv length, then data)
        MPI_Bcast(&bufferSize, 1, MPI_INT, 0, MPI_COMM_WORLD);

        buff = new double[bufferSize];
        MPI_Bcast(buff, bufferSize, MPI_DOUBLE, 0, MPI_COMM_WORLD);

        // ++ DESERIALIZATION clusters recvd
        Cluster::deserializeCluster(buff);
        // ---
        list<Point*> points;

        while (true) {
            Cluster::clustersReset();

            // ASSIGN POINTS TO CLUSTERS WITH NEAREST CENTROID
            Cluster::pointAssignment();
            // CALCULATE SLAVE SUM_CLUSTER
            Cluster::sumPointsClusters();

            // SEND SUM_CLUSTER AND TOTAL NUMBER OF POINTS
            int cluster_dim = Cluster::getThCluster(0)->getCentroid()->getDim();
            int cluster_number = Cluster::getNumberCluster();

            bufferSize = cluster_number * cluster_dim + cluster_number;

            buff = new double[bufferSize]; // buffer send
            buffer2 = new double[bufferSize]; // buffer recv, useless for slave

            Cluster::serializeSumClusters(buff);
            MPI_Reduce(buff, buffer2, bufferSize, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

            // CALCULATE MSE (Already did during assignment)
            // Send SumDistance
            buff = new double[1];
            buffer2 = new double[1]; // buffer recv, useless for slave
            buff[0] = Cluster::getSumDistance();

            MPI_Reduce(buff, buffer2, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

            // RECV FINISH CONDITION
            MPI_Bcast(&finish, 1, MPI_INT, 0, MPI_COMM_WORLD);
            if(finish){ break;}

            // RECV NEW CENTROIDS
            MPI_Bcast(&bufferSize, 1, MPI_INT, 0, MPI_COMM_WORLD);
            buff = new double[bufferSize];
            MPI_Bcast(buff, bufferSize, MPI_DOUBLE, 0, MPI_COMM_WORLD);

            Cluster::deSerializeCentroids(buff);
        }
    }

    MPI_Finalize();
    return 0;
}