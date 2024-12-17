> Branch con il codice modificato leggermente con l'aiuto di Copilot. <br>
> Altre cose sono state modificate per funzionare con il Debugger di CLion.

# Indice

- [Indice](#indice)
  - [Tecniche di profiling](#tecniche-di-profiling)
  - [Visualizzare perf.data](#visualizzare-perfdata)
  - [Per creare un Diagramma UML con `clang`](#per-creare-un-diagramma-uml-con-clang)
  - [Per debuggare il codice parallelo con CLion](#per-debuggare-il-codice-parallelo-con-clion)
  - [Setup ambiente di progetto su g\_cloud](#setup-ambiente-di-progetto-su-g_cloud)
  
## Tecniche di profiling 

Con **perf** (in uso)

```Shell
perf record -g mpirun --hostfile hostfile -np 32 ./kmeans_parallel
perf report # per visualizzare il risultato
```
Altro esempio:
```Shell
perf record -F 800 --call-graph dwarf -g mpirun --hostfile ../hostfile -np $((2*8)) ./kmeans_parallel --event
hotspot perf.data # per visualizzarlo
```
- `-F 800` per specificare la frequenza di campionamento (e.g meglio avere una -F bassa ~150, per generare un file di dimensioni contenute)
- `--call-graph dwarf` serve ad [hotspot](https://github.com/KDAB/hotspot) per visualizzare il report
- `--event` ??

Con **gprof** (non usato)

[Info su gprof in parallelo](https://stackoverflow.com/questions/53794093/how-do-i-get-meaningful-results-from-gprof-on-an-mpi-code)

```Shell
mpic++  -pg Centroid.cpp Cluster.cpp kmeans_parallel.cpp Point.cpp Tupla.cpp -o kmeans_parallel -lm
mpirun --hostfile ../hostfile -np $((2*16)) kmeans_parallel

gprof ./kmeans_parallel gmon.out > profiling_report.txt
```

## Visualizzare `perf.data`

Profiler di esecuzione su due macchine e2-standard-8 con un dataset di 50k10 e 10 iter.

Questo per copiare file da VM in g_cloud al pc
```Shell
gcloud compute scp --recurse ^
    instance0-fat-intra:"/home/galan/ACAproject/MPI-K-Means-Clustering-main/Parallel/test.perf" ^
    "C:\Users\galan\Desktop"
```
E invece questo nella stessa dir di `perf.data`, per visualizzarlo su [Firefox profiler](https://profiler.firefox.com/from-file/calltree/?globalTrackOrder=0w9&hiddenGlobalTracks=01&hiddenLocalTracksByPid=1561-0~1565-0&thread=2&timelineType=category&v=10)
```Shell
perf script -F +pid > test.perf
```
Mentre per generare un .svg con il FlameGraph di perf.data possiamo dare questi (src [cpuflamegraphs.html](https://www.brendangregg.com/FlameGraphs/cpuflamegraphs.html#Instructions))
```
./stackcollapse-perf.pl test.perf > out.perf-folded
./flamegraph.pl out.perf-folded > perf-flamegraph.svg
```

## Per creare un Diagramma UML con `clang`

Nella dir del progetto..

1. Creare cartelle **uml** e **build**
2. Dare ` cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=1 .` per generare `compile_commands.json` (1)
3. Inserire (1) in **build**
4. Scrivere `.clang-uml` così:
```yaml
---
compilation_database_dir: build
output_directory: uml
diagrams:
  parallel_class_diagram:
    type: class
    glob:
      - "*.cpp"
      - "*.h"
```
5. Dare `clang-uml` e caricare su https://planttext.com/ per ottenere l'uml in immagine
  
## Per debuggare il codice parallelo con CLion

```
C:\Users\galan\Documents\GitHub\ACAproject\MPI-K-Means-Clustering-main\Parallel\cmake-build-debug
```
1. Del tipo ![Pasted image 20241125113705|350](https://github.com/user-attachments/assets/8e2e477a-7a10-4e9d-a376-431774d4bb0a)
2. Inserire _dummy loop_ e un breakpoint su `sleep(1)`
```C++
int debug = 0;
while(debug == 0){
	sleep(1);
}
```
3. **Attatch to process** del tipo ![Pasted image 20241125115132|350](https://github.com/user-attachments/assets/593db1f5-959e-4531-bd8b-5d21762490eb)
4. Settare in CLion `debug` a 1 per andare avanti nell'esecuzione

## Setup ambiente di progetto su g_cloud

File necessari:
- **setup_proj.sh** per fare l'init della macchina con il necessario
- **test_conn.sh** per controllare se riesce a stabilire una connessione con le altre macchine

Tasks:
 1.  **setup_proj.sh**
 	1. nel caso dare `source .bashrc` sulla vm / padre delle altre 
 2.  dare `git checkout copilot-refactor` sulla repo
 	1. nel caso ricompilare **Serial** e **Parallel**
 3.  in host .. `scp -i id_rsa id_rsa* galan@<ip della vm padre>:/home/galan/.ssh`
 4.  `sudo chmod 600 id_rsa`
 5.  creo gruppo di instances e compilo l'`hostfile` con ip locale e `slots=<num proc>`
 6.  **test_conn.sh**
 7.  `time ./run_test_kmeans $((<num instances> * <num processes>))`
