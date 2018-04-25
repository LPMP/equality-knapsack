
#--------------------------------------------------------------
# TCLAP Library
#--------------------------------------------------------------
include_directories("${CMAKE_CURRENT_BINARY_DIR}/Dependencies/Source/TCLAP_Project/include")

#--------------------------------------------------------------
# Meta Library
#--------------------------------------------------------------
include_directories("${CMAKE_CURRENT_BINARY_DIR}/Dependencies/Source/Meta_Project/include")

#--------------------------------------------------------------
# SQLITE3 Library
#--------------------------------------------------------------
if(WITH_SQLITE3)
  FIND_LIBRARY(SQLITE3_LIB
    NAMES sqlite3
    PATHS ENV LD_LIBRARY_PATH)
endif()

#--------------------------------------------------------------
# PYBIND11 Library
#--------------------------------------------------------------

add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/Dependencies/Source/PYBIND11_Project)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/Dependencies/Source/PYBIND11_Project/include)
include_directories(/usr/include/python3.5)

#--------------------------------------------------------------
# CATCH Library
#--------------------------------------------------------------
FIND_PATH(CATCH_INCLUDE
  NAMES catch.hpp
  PATHS ${CMAKE_CURRENT_BINARY_DIR}/Dependencies/Source/CATCH_Project/include
  NO_CMAKE_PATH
  NO_SYSTEM_ENVIRONMENT_PATH
  NO_CMAKE_SYSTEM_PATH)
MARK_AS_ADVANCED(CATCH_INCLUDE)

#--------------------------------------------------------------
# GUROBI Library
#--------------------------------------------------------------
if(WITH_GUROBI)
  find_package(Gurobi)
endif(WITH_GUROBI)

#--------------------------------------------------------------
# MATLAB Library
#--------------------------------------------------------------
if(WITH_MATLAB)
  message(STATUS "build with MatLab support")
  find_package(Matlab REQUIRED)
  message(STATUS "Matlab include dir: ${MATLAB_INCLUDE_DIR}")
  message(STATUS "Matlab libmex: ${MATLAB_MEX_LIBRARY}")
  message(STATUS "Matlab libmat: ${MATLAB_MAT_LIBRARY}")
  message(STATUS "Matlab libmx:  ${MATLAB_MX_LIBRARY}")
endif(WITH_MATLAB)

#--------------------------------------------------------------
# PEGTL Library
#--------------------------------------------------------------
include_directories("${CMAKE_CURRENT_BINARY_DIR}/Dependencies/Source/PEGTL_Project/include")

#--------------------------------------------------------------
# PostgreSql Library
#--------------------------------------------------------------
if(WITH_LIBPQXX)
FIND_LIBRARY(PQXX_LIB
  NAMES pqxx
  DOC "library needed to connect to postgresql"
  PATHS ENV LD_LIBRARY_PATH)
FIND_LIBRARY(PQ_LIB
  NAMES pq
  DOC "library needed to connect to postgresql"
  PATHS ENV LD_LIBRARY_PATH)
FIND_PATH(PQXX_INCLUDE
  DOC "include needed to connect to postgresql"
  NAMES pqxx)
endif()
#--------------------------------------------------------------

