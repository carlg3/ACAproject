#include <iostream>
#include <stdlib.h>
#include <bits/stdc++.h>

#include <mpi.h>

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/set.hpp>
#include <boost/iostreams/stream_buffer.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/back_inserter.hpp>

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
        double starttime, endtime;  // Time variables
        starttime = MPI_Wtime();    // Start timer

        // INIT
        //-------------------------------------------------------------------
        // READING FILE
        vector<Point*> points_temp_;
        readDataSet(points_temp_,"/mnt/c/Users/galan/CLionProjects/Serial-proj-test/dataset/dataset_100x2.txt");

        int pointDimension = points_temp_[0]->get_dim();  // Dimensione del dato R^pointDimension
        int totalNumberPoint = points_temp_.size();      // Numero di dati nel nostro DataSet

        // INIT CLUSTERS AND CENTROIDS
        int K = sqrt(totalNumberPoint/2);
        Cluster::createKclusters(K, pointDimension);

        //-------------------------------------------------------------------
        // Sceglie quant'è grande il batch di punti su cui lavorerà ogni thread
        int pointsXprocessor = totalNumberPoint / commSize;

        // SERIALIZING AND SENDING POINTS
        // 2:
        //  buff[0]: ...
        //  buff[1]: ...
        // pointsXprocessor * pointDimension:
        //  . la lista dei punti di dimensione pointDimension
        int bufferSize = 2 + pointsXprocessor * pointDimension;

        double *buffer, *buffer2;
        buffer = new double[bufferSize];

        // Send length, then data
        for(int i = 1; i < commSize; i++){
            MPI_Send(&bufferSize, 1, MPI_INT, i, LENTAG, MPI_COMM_WORLD);

            // Qui serializza tutto in 'buffer', per inviare
            // buffer[0]: numero di punti da inviare
            // buffer[1]: dimensione del singolo punto
            // Perché prende prima l'indice del punto scelto nella lista 'points'
            // e poi prende le coordinate spostandosi a seconda della dimensione.

            // Quindi il primo thread prende un batch grande tot_,
            // il secondo thread prende l'altro batch spostandosi di tot_ punti nella lista, ecc..
            int startIndex = (i - 1) * pointsXprocessor;
            int endIndex = startIndex + pointsXprocessor;
            Point::serializePoint(buffer, startIndex, endIndex, pointDimension);

            // Dopo la 'serializePoint' buffer contiene i punti da inviare
            MPI_Send(buffer, bufferSize, MPI_DOUBLE, i, DATAPOINTTAG, MPI_COMM_WORLD);
        }

        // SEND CLUSTERS
        // 2:
        //  buff[0]: il numero dei cluster
        //  buff[1]: la dimensione dei punto centroide
        // K * pointDimension:
        //  . la lista dei centroidi di dimensione pointDimension

        delete[] buffer;
        buffer = nullptr;

        bufferSize = 2 + K * pointDimension;
        buffer = new double[bufferSize];

        MPI_Bcast(&bufferSize, 1, MPI_INT, 0, MPI_COMM_WORLD);

        // Quindi serializza tutto in 'buffer' da inviare a tutti i thread,
        // così tutti i thread sapranno quali sono i cluster e i rispettivi centroidi

        Cluster::serializeCluster(buffer);
        MPI_Bcast(buffer, bufferSize, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        // -------------------------------------------------------------------
        
        // INITIAL TMSE
        double previousTMSE, tmse = 0;
        int finish = 0;

        while(true) {
            Cluster::clustersReset();

            // ASSIGN POINTS TO CLUSTERS WITH NEAREST CENTROID
            endtime = MPI_Wtime(); // Stop timer
            printf("[INFO] Pre-Assignment: %f seconds\n", endtime - starttime); // Print execution time

            // Il master lo fa per l'ultimo batch di punti
            int startIndex = (commSize - 1) * pointsXprocessor;
            int endIndex = totalNumberPoint;
            Cluster::pointAssignment(startIndex, endIndex);

            endtime = MPI_Wtime(); // Stop timer
            printf("[INFO] Post-Assignment: %f seconds\n", endtime - starttime); // Print execution time

            // CALCULATE MASTER SUM_CLUSTER

            // Qui calcola la somma delle coordinate di tutti i punti dei cluster
            // quindi in ogni oggetto Cluster avrò un sumDistance di dimensione centroid_dim_
            // e ogni elemento sarà la somma
            Cluster::sumPointsClusters();
            
            // RECV SUM_CLUSTER AND NUMBER_OF_POINTS, AND CALCULATE TOTAL_SUM_CLUSTER AND TOTAL_NUMBER_OF_POINTS
            // 'bufferSize' è così perché
            // es.
            // ho 10 cluster, quindi i primi 10 elementi sono per salvare il numero di punti di ciascun cluster
            // gli altri 10*centroid_dim_ (== 10*2 == 20) per salvare la somma a due a due delle coordinate
            // quindi una coppia per ogni cluster

            int cluster_number_ = Cluster::getNumberCluster();
            int centroid_dim_ = pointDimension; // Dato che non possono esserci punti con dimensione diversa

            bufferSize = cluster_number_ + cluster_number_ * centroid_dim_;

            buffer = new double[bufferSize];    // buffer recv
            buffer2 = new double[bufferSize];   // buffer send, containing master data

            // Qui serializza la somma dei punti dei cluster in 'buffer2'
            // Quindi prima le somme delle coordinate dei punti di un cluster
            // es. con centroid_dim_ di 2 avrò i primi 2 elementi con la somma delle x e y del primo cluster
            // e poi il numero totale di punti del cluster e così via...

            Cluster::serializeSumClusters(buffer2);
            
            // 1--- REDUCE
            MPI_Reduce(buffer2, buffer, bufferSize, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

            // ++ DESERIALIZATION delle somme dei punti dei cluster
            Cluster::deserializeSumClusters(buffer);

            // CALCULATE NEW CENTROIDS

            // Setta il valore del nuovo centroide con la media dei punti nel cluster
            // lo fa per tutti i cluster, così da avere i nuovi centroidi rispetto a quelli assegnati random all'inizio
            Cluster::centroidsParallelAssignment();

            // CALCULATE TMSE
            // Recv SumDistance
            previousTMSE = tmse;

            delete[] buffer;
            buffer = nullptr;

            buffer = new double[1];     // buffer recv
            buffer2 = new double[1];    // buffer send

            buffer2[0] = Cluster::getSumDistance();
            
            // 2--- REDUCE
            MPI_Reduce(buffer2, buffer, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
            Cluster::setSumDistance(buffer[0]);

            tmse = Cluster::totalMSE();

            // SEND FINISH CONDITION
            if(!((MAXITERATION-- && tmse < previousTMSE) || previousTMSE == 0)){ break;}

            MPI_Bcast(&finish, 1, MPI_INT, 0, MPI_COMM_WORLD);

            // SEND NEW CENTROIDS
            bufferSize = cluster_number_ * centroid_dim_;
            buffer = new double[bufferSize];

            Cluster::serializeCentroids(buffer);
            
            MPI_Bcast(&bufferSize, 1, MPI_INT, 0, MPI_COMM_WORLD);
            MPI_Bcast(buffer, bufferSize, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        }

        finish = 1;
        MPI_Bcast(&finish, 1, MPI_INT, 0, MPI_COMM_WORLD);

        endtime = MPI_Wtime(); // Stop timer
        printf("That took %f seconds\n", endtime-starttime); // Print execution time

        Cluster::printCentroids();
    }

    if(my_rank != 0){
        int bufferSize;
        double *buffer, *buffer2;

        // RECV POINTS -- pointsXprocessor
        // Recv length, then data
        MPI_Recv(&bufferSize, 1, MPI_INT, 0, LENTAG, MPI_COMM_WORLD, &status);

        buffer = new double[bufferSize];
        MPI_Recv(buffer, bufferSize, MPI_DOUBLE, 0, DATAPOINTTAG, MPI_COMM_WORLD, &status);

        // DESERIALIZATION
        Point::deserializePoint(buffer);

        // RECV CLUSTERS
        // Recv length, then data
        MPI_Bcast(&bufferSize, 1, MPI_INT, 0, MPI_COMM_WORLD);

        delete[] buffer;
        buffer = nullptr;

        buffer = new double[bufferSize];

        MPI_Bcast(buffer, bufferSize, MPI_DOUBLE, 0, MPI_COMM_WORLD);

        // DESERIALIZATION
        Cluster::deserializeCluster(buffer);
        // -------------------------------------------------------------------

        int finish;

        delete[] buffer;
        buffer = nullptr;

        while(true) {
            Cluster::clustersReset();

            // ASSIGN POINTS TO CLUSTERS WITH NEAREST CENTROID
            Cluster::pointAssignment();

            // CALCULATE SLAVE SUM_CLUSTER
            Cluster::sumPointsClusters();

            // SEND SUM_CLUSTER AND TOTAL NUMBER OF POINTS
            int cluster_number_ = Cluster::getNumberCluster();
            int centroid_dim_ = Cluster::getThCluster(0)->getCentroid()->get_dim();

            bufferSize = cluster_number_ + cluster_number_ * centroid_dim_;

            buffer = new double[bufferSize]; // buffer send
            buffer2 = new double[bufferSize]; // buffer recv, useless for slave perché lo slave invierà i dati e non riceve
            Cluster::serializeSumClusters(buffer);

            // 1--- REDUCE
            MPI_Reduce(buffer, buffer2, bufferSize, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

            // CALCULATE MSE
            // Already did during assignment
            // Send SumDistance
            delete[] buffer;
            delete[] buffer2;
            buffer = nullptr;
            buffer2 = nullptr;

            buffer = new double[1];
            buffer2 = new double[1]; // buffer recv, useless for slave

            buffer[0] = Cluster::getSumDistance();

            // 2--- REDUCE
            MPI_Reduce(buffer, buffer2, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

            // RECV FINISH CONDITION
            MPI_Bcast(&finish, 1, MPI_INT, 0, MPI_COMM_WORLD);
            if(finish){ break;}

            // RECV NEW CENTROIDS
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