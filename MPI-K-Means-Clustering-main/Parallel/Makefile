# Makefile for compiling 'kmeans_sequential' program

TARGET = kmeans_parallel

SRCS = Centroid.cpp Cluster.cpp kmeans_parallel.cpp Point.cpp Tupla.cpp

OBJS = $(SRCS:.cpp=.o)

CXX = g++
CXXFLAGS = -Wall -O2

$(TARGET): $(OBJS)
	$(CXX) -o $(TARGET) $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: clean