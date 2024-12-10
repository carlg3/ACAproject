#!/bin/bash

# Script per fare il setup della prima VM 

# Verifica che lo script sia eseguito come root
if [[ $EUID -ne 0 ]]; then
   echo "Questo script deve essere eseguito come root." 
   exit 1
fi

# Installazione dei pacchetti necessari
echo "Installazione dei pacchetti necessari..."
yum install -y wget perl gcc gcc-c++ git

# Creazione delle directory necessarie
echo "Creazione delle directory per OpenMPI..."
mkdir -p /usr/local/openMPI
mkdir -p ~/openMPI

# Download e decompressione di Open MPI
echo "Download di Open MPI..."
cd ~/openMPI
wget https://download.open-mpi.org/release/open-mpi/v4.1/openmpi-4.1.1.tar.gz
tar -xvzf openmpi-4.1.1.tar.gz

# Compilazione e installazione di Open MPI
echo "Compilazione e installazione di Open MPI..."
cd openmpi-4.1.1
mkdir build
cd build
../configure --prefix=/usr/local/openMPI
make all install

# Ritorno all'utente normale
su - $SUDO_USER

# Configurazione dell'ambiente
echo "Configurazione del file .bashrc per OpenMPI..."
echo 'export PATH=$PATH:/usr/local/openMPI/bin' >> ~/.bashrc
source ~/.bashrc

# Clonazione del repository Git
echo "Clonazione del repository Git ACAproject..."
git clone https://github.com/carlg3/ACAproject.git
git checkout copilot-refactor

echo "Installazione completata!"
