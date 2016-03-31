cmake_minimum_required(VERSION 3.2)

PROJECT(nodeFactory)

#if you don't want the full compiler output, remove the following line
SET(CMAKE_VERBOSE_MAKEFILE ON)

#override CXX_FLAGS
# SET(CMAKE_CXX_FLAGS "-Wall -Os")

SET(CMAKE_CXX_STANDARD 14)
SET(CMAKE_CXX_STANDARD_REQUIRED on)

set(CMAKE_CXX_FLAGS ${DEFINITIONS_CXXFLAGS})

#list all source files here
AUX_SOURCE_DIRECTORY(. DB_SRC_FILES_CPP)
AUX_SOURCE_DIRECTORY(common COMMON_SRC_FILES_CPP)

# ADD_EXECUTABLE(nodeFactory ${DB_SRC_FILES_CPP} ${COMMON_SRC_FILES_CPP})

ADD_SUBDIRECTORY(client)
ADD_SUBDIRECTORY(server)

 
