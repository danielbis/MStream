cmake_minimum_required(VERSION 3.15)
project(mstream)

set(CMAKE_CXX_STANDARD 14)

add_executable(mstream main.cpp Document.cpp Document.h ClusterFeatureVector.cpp ClusterFeatureVector.h MStream.cpp MStream.h)