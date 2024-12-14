> Branch con il codice modificato leggermente con l'aiuto di Copilot. Altre cose sono state modificate per funzionare con il Debugger di CLion.

# Indice

- [Indice](#indice)
  - [Task per setup ambiente progetto](#task-per-setup-ambiente-progetto)
  - [Tecniche di profiling](#tecniche-di-profiling)
  - [Per creare un Diagramma UML con `clang`](#per-creare-un-diagramma-uml-con-clang)
  - [Per debuggare il codice parallelo con CLion](#per-debuggare-il-codice-parallelo-con-clion)

## Task per setup ambiente progetto

File necessari:
- **setup_proj.sh** 
- **test_conn.sh**

Tasks:
 1. [ ] **setup_proj.sh** -- e nel caso source .bashrc || vm padre delle altre 
 2. [ ] dare `git checkout copilot-refactor` sulla repo
	 1. [ ] nel caso ricompilare **Serial** e **Parallel**
 3. [ ] in host .. `scp -i id_rsa id_rsa* galan@<ip della vm padre>:/home/galan/.ssh`
 4. [ ] `sudo chmod 600 id_rsa`
 5. [ ] creo gruppo di instances e compilo l'`hostfile`
 6. [ ] **test_conn.sh**
 7. [ ] `time ./run_test_kmeans $((<num instances> * <num processes>))`

## Tecniche di profiling 

Con **gprof**

[Info su gprof in parallelo](https://stackoverflow.com/questions/53794093/how-do-i-get-meaningful-results-from-gprof-on-an-mpi-code)

```Shell
mpic++  -pg Centroid.cpp Cluster.cpp kmeans_parallel.cpp Point.cpp Tupla.cpp -o kmeans_parallel -lm
mpirun --hostfile ../hostfile -np $((2*16)) kmeans_parallel

gprof ./kmeans_parallel gmon.out > profiling_report.txt
```

Con **perf**

```Shell
perf record -g mpirun --hostfile hostfile -np 32 ./kmeans_parallel

perf report # per visualizzare il risultato
```
Inoltre

```Shell
perf record -F 800 --call-graph dwarf -g mpirun --hostfile ../hostfile -np 8 ./kmeans_parallel --event 
```
- `-F 800` per specificare la frequenza di campionamento
- `--call-graph dwarf` serve ad hotspot per visualizzare il report
- `--event` ??

quindi per visualizzarlo
```Shell
hotspot perf.data
```

## Per creare un Diagramma UML con `clang`

- Creare dir **uml** e **build**
- Dare ` cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=1 .` per generare `compile_commands.json` (1)
- Inserire (1) in **build**
- Scrivere `.clang-uml` così:
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
- Dare `clang-uml` e caricare su https://planttext.com/ per ottenere l'uml in immagine
  
## Per debuggare il codice parallelo con CLion

```
C:\Users\galan\Documents\GitHub\ACAproject\MPI-K-Means-Clustering-main\Parallel\cmake-build-debug
```
1. ![Pasted image 20241125113705|400](https://github.com/user-attachments/assets/8e2e477a-7a10-4e9d-a376-431774d4bb0a)
2. Inserire dummy loop e un breakpoint su `sleep(1)`
```C++
int debug = 0;
while(debug == 0){
	sleep(1);
}
```

4. **Attatch to process** del tipo ![Pasted image 20241125115132|400](https://github.com/user-attachments/assets/593db1f5-959e-4531-bd8b-5d21762490eb)
