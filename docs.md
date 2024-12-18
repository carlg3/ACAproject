### **Indice**

1. [Video esplicativo K-means](#video-esplicativo-k-means)  
2. [Perche' clustersReset](#perché-clustersreset)  
3. [Differenza tra sumDistance e sumClusters](#differenza-tra-sumdistance-e-sumclusters)  
4. [Differenza uso di list o vector](#differenza-uso-di-list-o-vector-seriale)  
5. [Assignment](#assignment)  
    - [PointAssignment](#pointassignment)  
    - [centroidParallelCalculator](#centroidparallelcalculator)  
6. [Serialization and deserialization](#serialization-and-deserialization)  
    - [serializeCentroids](#1-serializecentroids)  
    - [deserializeCentroids](#2-deserializecentroids)  
    - [serializeCluster](#3-serializecluster)  
    - [deserializeCluster](#4-deserializecluster)  
    - [serializeSumClusters](#5-serializesumclusters)  
    - [deserializeSumClusters](#6-deserializesumclusters)  
---
## Video esplicativo K-means

[Qui](https://youtu.be/4b5d3muPQmA?feature=shared) video ~8min di spiegazione sull'algoritmo **kmeans**

## Perché clustersReset

Nel codice, il metodo `Cluster::clustersReset()` viene chiamato all'inizio di ogni iterazione per assicurarsi che i cluster siano resettati e pronti per la nuova assegnazione di punti. Questo è necessario perché ogni iterazione del k-means deve ricalcolare l'assegnazione dei punti ai cluster, e quindi i cluster devono essere svuotati prima di aggiungere i nuovi punti.

Ecco perché è necessario resettare i cluster ad ogni iterazione:

1. **Pulizia dei dati precedenti**: Ogni iterazione del k-means riassegna tutti i punti ai cluster più vicini. Senza resettare i cluster, i punti delle iterazioni precedenti rimarrebbero nei cluster, falsando i risultati.
2. **Calcolo corretto delle nuove centroidi**: Le nuove centroidi vengono calcolate in base ai punti assegnati ai cluster in quella specifica iterazione. Quindi, è essenziale che i cluster siano vuoti all'inizio di ogni iterazione per calcolare correttamente le nuove centroidi.

## Differenza tra sumDistance e sumClusters

La differenza tra `sumDistance` e `sumClusters` nel contesto del k-means clustering è la seguente:

- **sumDistance**: Questo termine si riferisce alla somma delle distanze quadrate tra ogni punto e il suo centroide assegnato. Viene utilizzato per calcolare la misura della qualità dell'assegnazione dei punti ai cluster (ad esempio, l'errore quadratico medio totale, TMSE). Una somma di distanza più bassa indica una migliore qualità del clustering.

- **sumClusters**: Questo termine si riferisce alla somma dei vettori dei punti all'interno di ciascun cluster. Viene utilizzato per calcolare i nuovi centroidi dei cluster. Dopo che tutti i punti sono stati assegnati ai loro centroidi più vicini, il centroide di ciascun cluster viene aggiornato calcolando la media dei punti in quel cluster.

In sintesi:
- `sumDistance` misura la qualità del clustering.
- `sumClusters` viene utilizzato per aggiornare i centroidi dei cluster.
- 
## Differenza uso di `list` o `vector` (Seriale)

|       | Dataset    | Usando vettori (s) | Usando liste (s) |
| ----- | ---------- | ------------------ | ---------------- |
| **0** | `1000x10`  | 0.044              | 0.377            |
| **1** | `10000x10` | 0.307              | 83.24            |

## Assignment

### **PointAssignment**
```C++
void Cluster::pointAssignment(int startIndex, int endIndex) {  
    for (int i = startIndex; i < endIndex; i++) {
	    // Per inizializzare la ricerca  
        double minDistance = Point::getThPoint(i)->distanza(*clusters.front()->getCentroid());  
        Cluster* closestCluster = clusters.front();  
  
        for (auto cluster : clusters) {  
            double distance = Point::getThPoint(i)->distanza(*cluster->getCentroid());  
            if (distance < minDistance) {  
                minDistance = distance;  
                closestCluster = cluster;  
            }  
        }  
  
        closestCluster->addElement(Point::getThPoint(i));  
        Cluster::sumDistance += minDistance;  
    }  
}  
```
`*clusters.front()` ritorna il primo cluster della lista, mentre `distanza( .. )` la calcola dal centroide di quel cluster per il punto su cui è chiamata.

Nel `for (auto cluster : clusters)` calcola la distanza del punto $i$-esimo per ogni cluster, quindi trova quello più vicino al punto. Trovato il cluster più vicino, gli assegna il punto e aggiunge a `sumDistance` la distanza di quel punto dal centroide di quel cluster.

### **centroidParallelCalculator**
```C++
void Cluster::centroidParallelCalculator(){  
    int centroid_dim = centroid->getDim();  
    int number_elements = getNumberElements();  
  
    if(number_elements){  
        for(int i = 0; i < centroid_dim; i++){  
            centroid->setThValue(i, sumCluster[i]/number_elements);  
        }  
    }else{  
        for(int i = 0; i < centroid_dim; i++) {  
            centroid->setThValue(i,0);  
        }  
    }  
}  
```
La  `getNumberElements()` ritorna il numero di punti che ha il cluster su cui è chiamata.

La `centroidParallelCalculator` assegna un nuovo centroide a quel cluster su cui è chiamata, rispetto ai suoi punti.

Se è la prima volta che viene chiamata su quel cluster - magari è stato appena creatoe ha $0$ elementi - setta il centroide a $0$, sennò fa una cosa del genere: 
- esempio con $\Re^2$, quindi $(x,\ y)$ avrei $n$ punti e quindi `sumCluster` con $2$ elementi che saranno .. di ogni punto:
	0. `sumCluster[0]` $\rightarrow$ la somma delle $x$ 
	1. `sumCluster[1]` $\rightarrow$ la somma delle $y$ 
```C++  
void Cluster::centroidsParallelAssignment(){  
    for(int i = 0; i < Cluster::getNumberCluster(); i++){  
        Cluster::getThCluster(i)->centroidParallelCalculator();  
    }  
}
```
Assegna un nuovo centroide per ogni cluster.

## Serialization and deserialization

1.  Centroids
2.  Clusters
3.  SumCluster

### Risposta ChatGPT

Ecco una spiegazione sintetica di ciascuna funzione:
#### **1. `serializeCentroids(double* buffer)`**

```C++
void Cluster::serializeCentroids(double* buffer){  
    int K = Cluster::getNumberCluster();  
    int dim = Cluster::getThCluster(0)->getCentroid()->getDim();  
  
    for(int i = 0; i < K; i++){  
        for(int j = 0; j < dim; j++){  
            buffer[i * dim + j] = Cluster::getThCluster(i)->getCentroid()->getThValue(j);  
        }  
    }  
}
```

- **Cosa fa**: Copia i valori dei centroidi di tutti i cluster in un array di tipo `double`.
- **Come funziona**:
    - Per ogni cluster (`K` in totale) e per ogni coordinata del centroide (`dim` dimensioni), salva il valore della coordinata nel buffer.
    - Organizza i dati in modo lineare: il cluster `i` occupa gli indici da `i * dim` a `(i + 1) * dim - 1` nel buffer.

---

#### **2. `deserializeCentroids(double* buffer)`**

- **Cosa fa**: Ripristina i valori dei centroidi dei cluster da un array `double`.
- **Come funziona**:
    - Legge i valori dal buffer e li assegna ai centroidi dei cluster corrispondenti.
    - I dati sono estratti nello stesso ordine lineare usato in `serializeCentroids`.

---

#### **3. `serializeCluster(double* buffer)`**

```C++
void Cluster::serializeCluster(double* buffer){  
    int K = Cluster::getNumberCluster();  
    int dim = Cluster::getThCluster(0)->getCentroid()->getDim();  
  
    buffer[0] = K; // Number of klusters  
    buffer[1] = dim; // Centroid dimension  
  
    Cluster::serializeCentroids(buffer + 2);  
}
```

- **Cosa fa**: Salva nel buffer il numero di cluster (`K`), la dimensione dei centroidi (`dim`) e poi i valori dei centroidi.
- **Come funziona**:
    - Il primo elemento del buffer è il numero di cluster.
    - Il secondo elemento è la dimensione dei centroidi.
    - I successivi elementi contengono i centroidi, copiati usando `serializeCentroids`.

---

#### **4. `deserializeCluster(double* buffer)`**

- **Cosa fa**: Ricrea i cluster a partire dai dati salvati nel buffer.
- **Come funziona**:
    - Legge il numero di cluster (`K`) e la dimensione dei centroidi (`dim`) dai primi due elementi del buffer.
    - Crea i cluster chiamando `Cluster::createKclusters(K, dim)`.
    - Ripristina i centroidi usando `deserializeCentroids`.

---

#### **5. `serializeSumClusters(double* buffer)`**

```C++
void Cluster::serializeSumClusters(double *buffer){  
    int cluster_number_ = Cluster::getNumberCluster();  
    int dim = Cluster::getThCluster(0)->getCentroid()->getDim();  
    int index;  
  
    for(int i = 0; i < cluster_number_; i++){  
        index = i * (dim + 1);  
  
        for(int j = 0; j < dim; j++){  
            buffer[index + j] = Cluster::getThCluster(i)->getSumCluster(j);  
        }  
  
        buffer[index + dim] = Cluster::getThCluster(i)->getNumberElements();  
    }  
}
```

- **Cosa fa**: Salva la somma dei punti (`SumCluster`) e il numero di elementi di ciascun cluster in un buffer.
- **Come funziona**:
    - Per ogni cluster, salva prima i valori delle somme (`SumCluster`, `dim` valori per cluster) e poi il numero di elementi del cluster.
    - Ogni cluster occupa `dim + 1` posizioni nel buffer.

---

#### **6. `deserializeSumClusters(double* buffer)`**

- **Cosa fa**: Ripristina le somme dei punti (`SumCluster`) e il numero di elementi di ciascun cluster dal buffer.
- **Come funziona**:
    - Legge i valori dal buffer e li assegna ai corrispondenti cluster.
    - Per ogni cluster, aggiorna sia i valori della somma (`SumCluster`) che il numero di elementi.

---

Queste funzioni sono progettate per serializzare e deserializzare le informazioni necessarie a rappresentare lo stato dei cluster, consentendo di trasmetterle o salvarle in formato compatto e lineare.
