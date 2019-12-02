cmake_minimum_required(VERSION 3.8.2)
project(mstream)

set(CMAKE_CXX_STANDARD 14)

add_executable(main.cpp Document.cpp Document.h ClusterFeatureVector.cpp ClusterFeatureVector.h MStream.cpp MStream.h)