# Profiling test con 2 macchine da 16 vCore
echo 'Compiling...'
mpic++  -pg Centroid.cpp Cluster.cpp kmeans_parallel.cpp Point.cpp Tupla.cpp -o kmeans_parallel -lm
echo 'Running...'
mpirun --hostfile ../hostfile -np $((2*16)) kmeans_parallel
echo 'Saving gprof results...'
gprof ./kmeans_parallel gmon.out > profiling_report.txt
