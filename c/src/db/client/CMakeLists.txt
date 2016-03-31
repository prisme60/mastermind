cmake_minimum_required(VERSION 3.2)

PROJECT(client)

#if you don't want the full compiler output, remove the following line
SET(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED on)

ADD_DEFINITIONS(-DTEST_UNIT_DBCLIENT)

set(CMAKE_CXX_FLAGS ${DEFINITIONS_CXXFLAGS})

#list all source files here
AUX_SOURCE_DIRECTORY(. CLIENT_SRC_FILES_CPP)
ADD_EXECUTABLE(client ${CLIENT_SRC_FILES_CPP})
