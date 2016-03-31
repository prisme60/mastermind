cmake_minimum_required(VERSION 3.2)

PROJECT(mastermind)

#if you don't want the full compiler output, remove the following line
SET(CMAKE_VERBOSE_MAKEFILE ON)

#override CXX_FLAGS
# SET(CMAKE_CXX_FLAGS "-Wall -Os")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED on)

# Bump up warning levels appropriately for clang, gcc & msvc
# Also set debug/optimization flags depending on the build type. IDE users choose this when
# selecting the build mode in their IDE
if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -std=c++14")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG} -g")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE}")
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
	if (CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
		string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
	else()
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
	endif()
endif()


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

INCLUDE_DIRECTORIES(src/include)

#list all source files here
AUX_SOURCE_DIRECTORY(src SRC_FILES_CPP)
ADD_EXECUTABLE(mastermind ${SRC_FILES_CPP})

ADD_SUBDIRECTORY(src/db)
 
