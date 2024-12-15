> Branch con il codice modificato leggermente con l'aiuto di Copilot. Altre cose sono state modificate per funzionare con il Debugger di CLion.

# Indice

- [Indice](#indice)
  - [Task per setup ambiente di progetto su g\_cloud](#task-per-setup-ambiente-di-progetto-su-g_cloud)
  - [Tecniche di profiling](#tecniche-di-profiling)
  - [Visualizzare perf.data](#visualizzare-perf-data)
  - [Per creare un Diagramma UML con `clang`](#per-creare-un-diagramma-uml-con-clang)
  - [Per debuggare il codice parallelo con CLion](#per-debuggare-il-codice-parallelo-con-clion)
  
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
- `-F 800` per specificare la frequenza di campionamento
- `--call-graph dwarf` serve ad [hotspot](https://github.com/KDAB/hotspot) per visualizzare il report
- `--event` ??

Con **gprof** (non usato)

[Info su gprof in parallelo](https://stackoverflow.com/questions/53794093/how-do-i-get-meaningful-results-from-gprof-on-an-mpi-code)

```Shell
mpic++  -pg Centroid.cpp Cluster.cpp kmeans_parallel.cpp Point.cpp Tupla.cpp -o kmeans_parallel -lm
mpirun --hostfile ../hostfile -np $((2*16)) kmeans_parallel

gprof ./kmeans_parallel gmon.out > profiling_report.txt
```

## Visualizzare perf data

Profiler di esecuzione su due macchine e2-standard-8 con un dataset di 50k10 e 10 iter.

[cpuflamegraphs.html](https://www.brendangregg.com/FlameGraphs/cpuflamegraphs.html)

Questo per copiare file da VMgcloud a pc
```Shell
gcloud compute scp --recurse ^
	instance0-fat-intra:"/home/galan/ACAproject/MPI-K-Means-Clustering-main/Parallel/test.perf" ^
	"C:\Users\galan\Desktop"
```
E invece questo nella stessa dir di `perf.data`, per visualizzarlo su **Firefox profiler**
```Shell
perf script -F +pid > /tmp/test.perf
```

[Firefox profiler](https://profiler.firefox.com/from-file/calltree/?globalTrackOrder=0w9&hiddenGlobalTracks=01&hiddenLocalTracksByPid=1561-0~1565-0&thread=2&timelineType=category&v=10)

## Per creare un Diagramma UML con `clang`

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
1. ![Pasted image 20241125113705|400](https://github.com/user-attachments/assets/8e2e477a-7a10-4e9d-a376-431774d4bb0a)
2. Inserire _dummy loop_ e un breakpoint su `sleep(1)`
```C++
int debug = 0;
while(debug == 0){
	sleep(1);
}
```
3. **Attatch to process** del tipo ![Pasted image 20241125115132|400](https://github.com/user-attachments/assets/593db1f5-959e-4531-bd8b-5d21762490eb)
4. Settare in CLion `debug` a 1 per andare avanti nell'esecuzione

## Task per setup ambiente di progetto su g_cloud

File necessari:
- **setup_proj.sh** 
- **test_conn.sh**

Tasks:
 1. [ ] **setup_proj.sh** -- e nel caso dare `source .bashrc` sulla vm / padre delle altre 
 2. [ ] dare `git checkout copilot-refactor` sulla repo
	 1. [ ] nel caso ricompilare **Serial** e **Parallel**
 3. [ ] in host .. `scp -i id_rsa id_rsa* galan@<ip della vm padre>:/home/galan/.ssh`
 4. [ ] `sudo chmod 600 id_rsa`
 5. [ ] creo gruppo di instances e compilo l'`hostfile`
 6. [ ] **test_conn.sh**
 7. [ ] `time ./run_test_kmeans $((<num instances> * <num processes>))`
