# Makefile for compiling 'kmeans_sequential' program

TARGET = kmeans_parallel.o

SRCS = Centroid.cpp Cluster.cpp kmeans_parallel.cpp Point.cpp Tupla.cpp

CXX = mpic++ 

$(TARGET): 
	$(CXX) $(SRCS) -o $(TARGET) -lm

clean:
	@echo "Pulizia del file.."
	rm $(TARGET)
