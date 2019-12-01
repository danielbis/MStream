CC = g++
CFLAGS = -g -Wall -std=c++14 -03

mstream: main.o MStream.o ClusterFeatureVector.o Document.o
	g++ -o mstream main.o MStream.o ClusterFeatureVector.o Document.o

main.o: main.cpp Document.h MStream.h
	g++ -c main.cpp

Mstream.o: MStream.cpp MStream.h ClusterFeatureVector.h Document.h
	g++ -c MStream.cpp

ClusterFeatureVector.o: ClusterFeatureVector.cpp ClusterFeatureVector.h Document.h
	g++ -c ClusterFeatureVector.cpp

Document.o: Document.cpp Document.h
	g++ -c Document.cpp

clean:
	rm *.o 