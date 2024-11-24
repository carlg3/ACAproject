#!/bin/bash

# Controlla che sia stato passato un argomento
if [ $# -ne 1 ]; then
    echo "Uso: $0 <numero_max_np>"
    exit 1
fi

# Numero massimo di processi da utilizzare
MAX_NP=$1

# File hostfile e percorso del file eseguibile
HOSTFILE="../hostfile"
EXECUTABLE="kmeans_parallel"

# Esegui il comando per np da 1 a MAX_NP
for np in $(seq 1 $MAX_NP)
do
    echo "Esecuzione con -np $np..."
    mpirun --hostfile $HOSTFILE -np $np $EXECUTABLE
done
