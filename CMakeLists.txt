# -----------------------------------------------------------------
# Project -> Equality Knapsack Problem
# -----------------------------------------------------------------
cmake_minimum_required(VERSION 2.6)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)
project(EKP)
set(MAP_VERSION_MAJOR 1)
set(MAP_VERSION_MINOR 0)

#--------------------------------------------------------------
# global headers
#--------------------------------------------------------------
file(GLOB_RECURSE headers include/*.hxx)
include_directories(include)

#--------------------------------------------------------------
# C++14 Flag
#--------------------------------------------------------------
SET(C++_STD_FLAG "c++14")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=${C++_STD_FLAG}")
MARK_AS_ADVANCED(C++_STD_FLAG)

#--------------------------------------------------------------
# OPTIONS
#--------------------------------------------------------------
OPTION(DOWNLOAD_DEPENDENCIES "Download and compile dependencies automatically" ON)
OPTION(DOWNLOAD_LIBPQXX "Download API for PostgreSql" OFF)
OPTION(BUILD_TESTS "Build Unit Tests" ON)
OPTION(WITH_LIBPQXX "Activate PostgreSql-Code" OFF)
OPTION(WITH_MATLAB "Activate MatLab-Code" OFF)
OPTION(WITH_GUROBI "Activate Gurobi-Code" OFF)
OPTION(WITH_SQLITE3 "Activate Sqlite3-Code" OFF)

#--------------------------------------------------------------
# add directories
#--------------------------------------------------------------

if(DOWNLOAD_DEPENDENCIES)
  project(DOWNLOAD_DEPENDENCIES_PROJECT)
  include(cmake/LoadDependencies.cmake)
else()
  include(cmake/CreateMakefiles.cmake)
  add_subdirectory(src)  
endif()

