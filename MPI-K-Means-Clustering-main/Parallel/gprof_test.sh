#!/bin/bash

# Test di profiling con gprof (-pg flag)

# Controlla che sia stato passato un argomento
if [ $# -ne 1 ]; then
    echo "Uso: $0 <numero_max_np>"
    exit 1
fi

# Numero massimo di processi da utilizzare
MAX_NP=$1

mpic++ -pg Centroid.cpp Cluster.cpp kmeans_parallel.cpp Point.cpp Tupla.cpp -o kmeans_parallel -lm
mpirun --hostfile ../hostfile -np $MAX_NP kmeans_parallel

gprof ./kmeans_parallel gmon.out > profiling_report.txt