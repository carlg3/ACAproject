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

string dataset = path_gcloud + "DataSet/DataSet1000x2.txt";

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
            point->set_value(i, stod(tokens[i]));
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

        // Reading the dataset...
        vector<Point*> points_temp_;
		if(argc == 2){
			// Se ho passato un argomento
			dataset = path_gcloud + "DataSet/" + argv[1];
		}
        readDataSet(points_temp_, dataset);

        // After reading the dataset
        start_time = MPI_Wtime();

        int pointDimension = points_temp_[0]->get_dim();     // Dimensione del dato R^pointDimension
        int totalNumberPoint = (int)points_temp_.size();    // Numero di dati nel nostro DataSet

        int K = sqrt(totalNumberPoint/2);

        Cluster::create_clusters(K, pointDimension);

        // Cluster::saveCentroids(my_rank, 90);

        int pointsXprocessor = totalNumberPoint / commSize;
        int bufferSize = 2 + pointsXprocessor * pointDimension;

        double *buffer, *buffer2;
        buffer = new double[bufferSize];

        // Sends derived points to each other processor
        MPI_Bcast(&bufferSize, 1, MPI_INT, 0, MPI_COMM_WORLD);

        /*
         * Parto da 1 perché il padre ha già la sua parte di punti
         * ed invia ad ogni figlio la sua parte
        */

        for(int i = 1; i < commSize; i++){
            int startIndex = (i - 1) * pointsXprocessor;
            int endIndex = startIndex + pointsXprocessor;
            Point::serializePoint(buffer, startIndex, endIndex, pointDimension);

            MPI_Send(buffer, bufferSize, MPI_DOUBLE, i, DATAPOINTTAG, MPI_COMM_WORLD);
        }

        delete[] buffer; buffer = nullptr;

        bufferSize = 2 + K * pointDimension;
        buffer = new double[bufferSize];

        MPI_Bcast(&bufferSize, 1, MPI_INT, 0, MPI_COMM_WORLD);

        // Sends number of clusters and init centroids
        Cluster::serializeCluster(buffer);
        MPI_Bcast(buffer, bufferSize, MPI_DOUBLE, 0, MPI_COMM_WORLD);

        // Start of the task
        double previousTMSE, tmse = 0;
        int finish = 0;

        // Cluster::saveCentroids(my_rank, 130);

        while(true) {
            Cluster::reset_clusters();

            Cluster::saveCentroids(my_rank, 131);

            // Master works on the last batch of points <processors_point>
            int startIndex = (commSize - 1) * pointsXprocessor;
            int endIndex = totalNumberPoint;
            Cluster::map_point_to_cluster(startIndex, endIndex);

            // Gets the sum of the points of each cluster (of the master's points)
            Cluster::sum_points_clusters();

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
            Cluster::find_centroid_clusters();

            // new TMSE used by the stopping criterion
            previousTMSE = tmse;

            delete[] buffer; buffer = nullptr;

            buffer = new double[1];     // recv
            buffer2 = new double[1];    // send

            // Gets the static value sumDistance of all cluster (of the master's points)
            buffer2[0] = Cluster::getSumDistance();

            // 2--- REDUCE
            MPI_Reduce(buffer2, buffer, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

            // Sets the sumDistance (in the master) derived by the sums of each slave's sumDistance
            Cluster::setSumDistance(buffer[0]);

            tmse = Cluster::totalMSE();

            // DEBUG -- CLion
            /*int debug = 0;
            while(debug == 0) {
                sleep(1);
            }*/

            // Check finish conditions
            if(!((MAXITERATION-- && tmse < previousTMSE) || previousTMSE == 0)){ break;}

            MPI_Bcast(&finish, 1, MPI_INT, 0, MPI_COMM_WORLD);

            // If not finished, calculates new centroids and sends them
            bufferSize = cluster_number_ * centroid_dim_;
            buffer = new double[bufferSize];

            Cluster::serializeCentroids(buffer);
            
            MPI_Bcast(&bufferSize, 1, MPI_INT, 0, MPI_COMM_WORLD);
            MPI_Bcast(buffer, bufferSize, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        }

        // End condition
        finish = 1;
        MPI_Bcast(&finish, 1, MPI_INT, 0, MPI_COMM_WORLD);

        end_time = MPI_Wtime();
        printf("That took %f seconds\n", end_time - start_time); // Print execution time

        /*
        // DEBUG -- CLion
        int debug = 0;
        while(debug == 0) {
            sleep(1);
        }
        */

        // DEBUG -- per salvare il tempo che ci si mette ad ogni esecuzione [MASTER]
        // <processi-- commSize> <numero di punti-- totalNumberPoint> <dimensione punti-- pointDimension> <numero di cluster> <tempo di esecuzione>
        // writeExTime(commSize, totalNumberPoint, pointDimension, K, end_time - start_time);

        Cluster::saveCentroids(my_rank, 210);
        Cluster::saveClusters(my_rank, 220);
    }

    if(my_rank != 0){
        int bufferSize;
        double *buffer, *buffer2;

        // Gets <processors_points>
        MPI_Bcast(&bufferSize, 1, MPI_INT, 0, MPI_COMM_WORLD);

        buffer = new double[bufferSize];
        MPI_Recv(buffer, bufferSize, MPI_DOUBLE, 0, DATAPOINTTAG, MPI_COMM_WORLD, &status);

        Point::deserializePoint(buffer);

        // Gets number of clusters and centroids
        MPI_Bcast(&bufferSize, 1, MPI_INT, 0, MPI_COMM_WORLD);

        delete[] buffer; buffer = nullptr;

        buffer = new double[bufferSize];

        MPI_Bcast(buffer, bufferSize, MPI_DOUBLE, 0, MPI_COMM_WORLD);

        Cluster::deserializeCluster(buffer);

        int finish;

        delete[] buffer; buffer = nullptr;

        while(true) {
            Cluster::reset_clusters();

            // Assign points to clusters with the nearest centroid
            Cluster::map_point_to_cluster();

            // Calculates the sumPoints of the points that the slave is working with
            Cluster::sum_points_clusters();

            int cluster_number_ = Cluster::get_sclusters_();
            int centroid_dim_ = Cluster::get_cluster(0)->get_centroid()->get_dim();

            bufferSize = cluster_number_ + cluster_number_ * centroid_dim_;

            buffer = new double[bufferSize]; // send
            buffer2 = new double[bufferSize];
            Cluster::serializeSumClusters(buffer);

            // 1--- REDUCE
            MPI_Reduce(buffer, buffer2, bufferSize, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

            // CALCULATE MSE
            delete[] buffer; buffer = nullptr;
            delete[] buffer2; buffer2 = nullptr;

            buffer = new double[1];
            buffer2 = new double[1];

            // Get the sumDistance of the clusters
            buffer[0] = Cluster::getSumDistance();

            // 2--- REDUCE | Sends the master its sumDistance
            MPI_Reduce(buffer, buffer2, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

            // Get finish condition from master
            MPI_Bcast(&finish, 1, MPI_INT, 0, MPI_COMM_WORLD);
            if(finish){ break;}

            // If not finished, waits for new cluster centroids
            MPI_Bcast(&bufferSize, 1, MPI_INT, 0, MPI_COMM_WORLD);

            delete[] buffer; buffer = nullptr;

            buffer = new double[bufferSize];
            MPI_Bcast(buffer, bufferSize, MPI_DOUBLE, 0, MPI_COMM_WORLD);

            Cluster::deserializeCentroids(buffer);
        }

        Cluster::saveClusters(my_rank, 311);
        Cluster::saveCentroids(my_rank, 312);
    }

    MPI_Finalize();

    return 0;
}