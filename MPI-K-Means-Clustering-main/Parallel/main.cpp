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

int K = 0; // Number of Clusters
int MAXITERATION = 100;
const int lentag=0;
const int stat=1;
const int datapointtag=2;
const int dataclustertag=3;
const int datasumclustertag=4;

void readDataSet(int *pointDimension,int *totalNumberPoint){
    string buffer;
    ifstream DataSet; DataSet.open("DataSet10000x10.txt");
    if(!DataSet.is_open()){
        cout << "FILE OPENING FAILED" << endl;
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
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

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);
    MPI_Status status;
    srand(time(0)); // Randomize initialization point
    // Get my rank
    int my_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    int commSize;
    MPI_Comm_size(MPI_COMM_WORLD, &commSize);
    if (my_rank == 0) {
        double starttime, endtime; // Time variables
        starttime = MPI_Wtime(); // Start timer
        int pointDimension;    // Dimensione del dato R^pointDimension
        int totalNumberPoint = 0; // Numbers of data in our DataSet
        // INIZIALIZETION
        //-------------------------------------------------------------------
        // READING FILE
        readDataSet(&pointDimension,&totalNumberPoint);
        // INIZIALIZE CLUSTERS AND CENTROIDS
        K = sqrt(totalNumberPoint/2);
        Cluster::createKclusters(K,pointDimension);
        //-------------------------------------------------------------------
        int pointsXprocessor = totalNumberPoint / commSize;
        // SERIALIZING AND SENDING POINTS
        int bufferSize = 2+pointsXprocessor*pointDimension;
        double *buffer;
        double* buffer2;
        buffer = new double[bufferSize];
        // Send length, then data
        for(int i=1;i<commSize;i++){
            MPI_Send(&bufferSize,1,MPI_INT,i,lentag,MPI_COMM_WORLD);
            Point::serializePoint(buffer,(i-1)*pointsXprocessor,(i-1)*pointsXprocessor+pointsXprocessor,pointDimension);
            MPI_Send(buffer,bufferSize,MPI_DOUBLE,i,datapointtag,MPI_COMM_WORLD);
        }
        // SEND CLUSTERS
        bufferSize = 2+K*pointDimension;
        buffer = new double[bufferSize];
        MPI_Bcast(&bufferSize, 1, MPI_INT, 0, MPI_COMM_WORLD);
        Cluster::serializeCluster(buffer, K, pointDimension);
        MPI_Bcast(buffer, bufferSize, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        list<Point*> points;
        int finish = 0;
        // INITIAL TMSE
        double previousTMSE = 0;
        double tmse = 0;
        while(true) {
            Cluster::clustersReset();
            // ASSIGN POINTS TO CLUSTERS WITH NEAREST CENTROID
            endtime   = MPI_Wtime(); // Stop timer
            printf("APRE ASS %f seconds\n",endtime-starttime); // Print execution time
            Cluster::pointAssignment((commSize-1)*pointsXprocessor,totalNumberPoint);
            endtime   = MPI_Wtime(); // Stop timer
            printf("AFTER ASS %f seconds\n",endtime-starttime); // Print execution time
            // CALCULATE MASTER SUM_CLUSTER
            for(int i=0;i<Cluster::getNumberCluster();i++){
                Cluster::sumPointsClusters();
            }
            // RECV SUM_CLUSTER AND NUMBER_OF_POINTS, AND CALCULATE TOTAL_SUM_CLUSTER AND TOTAL_NUMBER_OF_POINTS
            bufferSize = Cluster::getThCluster(0)->getCentroid()->getDim()*Cluster::getNumberCluster()+Cluster::getNumberCluster();
            buffer = new double[bufferSize]; // buffer recv
            buffer2 = new double[bufferSize]; // buffer send, containing master data
            Cluster::serializeSumClusters(buffer2);
            MPI_Reduce(buffer2, buffer, bufferSize, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
            for(int i=0;i<Cluster::getNumberCluster();i++) {
                for (int j=0; j < Cluster::getThCluster(0)->getCentroid()->getDim(); j++) {
                    Cluster::getThCluster(i)->setSumCluster(j,buffer[i*(Cluster::getThCluster(i)->getCentroid()->getDim()+1)+j]);
                }
                Cluster::getThCluster(i)->setNumberElements(buffer[i*(Cluster::getThCluster(i)->getCentroid()->getDim()+1)+Cluster::getThCluster(i)->getCentroid()->getDim()]);
            }
            // CALCULATE NEW CENTROIDS
            Cluster::centroidsParallelAssignment();
            // CALCULATE TMSE
            // Recv SumDistance
            previousTMSE = tmse;
            buffer = new double[1]; // buffer recv
            buffer2 = new double[1]; // buffer send
            buffer2[0] = Cluster::getSumDistance();
            MPI_Reduce(buffer2, buffer, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
            Cluster::setSumDistance(buffer[0]);
            tmse = Cluster::totalMSE();
            // SEND FINISH CONDITION
            if(!((MAXITERATION-- && tmse<previousTMSE) || previousTMSE==0)){
                break;
            }
            MPI_Bcast(&finish, 1, MPI_INT, 0, MPI_COMM_WORLD);
            // SEND NEW CENTROIDS
            bufferSize = Cluster::getNumberCluster()*pointDimension;
            buffer = new double[bufferSize];
            Cluster::serializeCentroids(buffer);
            MPI_Bcast(&bufferSize, 1, MPI_INT, 0, MPI_COMM_WORLD);
            MPI_Bcast(buffer, bufferSize, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        }
        finish = 1;
        MPI_Bcast(&finish, 1, MPI_INT, 0, MPI_COMM_WORLD);
        endtime   = MPI_Wtime(); // Stop timer
        printf("That took %f seconds\n",endtime-starttime); // Print execution time
    }

    if(my_rank != 0){
        int bufferSize;
        double* buffer;
        double* buffer2;
        // RECV POINTS
        // Recv length, then data
        MPI_Recv(&bufferSize,1,MPI_INT,0,lentag,MPI_COMM_WORLD,&status);
        buffer = new double[bufferSize];
        MPI_Recv(buffer,bufferSize,MPI_DOUBLE,0,datapointtag,MPI_COMM_WORLD,&status);
        // DESERIALIZATION
        Point::deserializaPoint(buffer);
        // RECV CLUSTERS
        // Recv length, then data
        MPI_Bcast(&bufferSize, 1, MPI_INT, 0, MPI_COMM_WORLD);
        buffer = new double[bufferSize];
        MPI_Bcast(buffer, bufferSize, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        // DESERIALIZATION
        Cluster::deserializeCluster(buffer);
        list<Point*> points;
        int finish;
        while(true) {
            Cluster::clustersReset();
            // ASSIGN POINTS TO CLUSTERS WITH NEAREST CENTROID
            Cluster::pointAssignment();
            // CALCULATE SLAVE SUM_CLUSTER
            Cluster::sumPointsClusters();
            // SEND SUM_CLUSTER AND TOTAL NUMBER OF POINTS
            bufferSize = Cluster::getNumberCluster()*Cluster::getThCluster(0)->getCentroid()->getDim()+Cluster::getNumberCluster();
            buffer = new double[bufferSize]; // buffer send
            buffer2 = new double[bufferSize]; // buffer recv, useless for slave
            Cluster::serializeSumClusters(buffer);
            MPI_Reduce(buffer, buffer2, bufferSize, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
            // CALCULATE MSE
            // Already did during assignment
            // Send SumDistance
            buffer = new double[1];
            buffer2 = new double[1]; // buffer recv, useless for slave
            buffer[0] = Cluster::getSumDistance();
            MPI_Reduce(buffer, buffer2, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
            // RECV FINISH CONDITION
            MPI_Bcast(&finish, 1, MPI_INT, 0, MPI_COMM_WORLD);
            if(finish){
                break;
            }
            // RECV NEW CENTROIDS
            MPI_Bcast(&bufferSize, 1, MPI_INT, 0, MPI_COMM_WORLD);
            buffer = new double[bufferSize];
            MPI_Bcast(buffer, bufferSize, MPI_DOUBLE, 0, MPI_COMM_WORLD);
            Cluster::deSerializeCentroids(buffer);
        }
    }
    MPI_Finalize();
    return 0;
}
