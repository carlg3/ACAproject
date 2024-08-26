# Makefile for compiling 'kmeans_sequential' program

TARGET = kmeans_parallel

SRCS = Centroid.cpp Cluster.cpp kmeans_parallel.cpp Point.cpp Tupla.cpp

#OBJS = $(SRCS:.cpp=.o)

CXX = mpic++ 
#CXXFLAGS = -Wall -O2

$(TARGET): $(SRCS)
	$(CXX) $(SRCS) -o $(TARGET) -lm

#%.o: %.cpp
#	$(CXX) $(CXXFLAGS) -c $< -o $@

#clean:
#rm -f $(OBJS) $(TARGET)

.PHONY: clean
