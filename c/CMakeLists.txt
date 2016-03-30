cmake_minimum_required(VERSION 3.2)

PROJECT(mastermind)

#if you don't want the full compiler output, remove the following line
SET(CMAKE_VERBOSE_MAKEFILE ON)

#override CXX_FLAGS
SET(CMAKE_CXX_FLAGS "-Wall -Os")

OPTION (USE_OpenMP "Use OpenMP" OFF)
IF(USE_OpenMP)
    FIND_PACKAGE(OpenMP)
    IF(OPENMP_FOUND)
        MESSAGE("OpenMP found!!!!")
        SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_CXX_FLAGS}")
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    ENDIF(OPENMP_FOUND)
ENDIF()


#add definitions, compiler switches, etc.
#ADD_DEFINITIONS(-Wall) #debug
#ADD_DEFINITIONS(-Wall -DNO_RESULT_OUTPUT)

set(CMAKE_CXX_FLAGS ${DEFINITIONS_CXXFLAGS})

#list all source files here
AUX_SOURCE_DIRECTORY(src SRC_FILES_CPP)
ADD_EXECUTABLE(mastermind ${SRC_FILES_CPP})

#need to link to some other libraries ? just add them here
#TARGET_LINK_LIBRARIES(labyrinthe png jpeg)
#TARGET_LINK_LIBRARIES(mastermind ${SDL_LIBRARY})
 
