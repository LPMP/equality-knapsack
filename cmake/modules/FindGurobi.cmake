SET(Gurobi_FOUND 0)

SET(GUROBI_ROOT "" CACHE PATH "Gurobi root directory")

FIND_LIBRARY(GUROBI_LIB
  NAMES libgurobi65 gurobi65
  HINTS ${GUROBI_ROOT}/lib
  PATHS /usr/lib
        ENV LIBRARY_PATH
        ENV LD_LIBRARY_PATH)

FIND_LIBRARY(GUROBI_LIB_C++
  NAMES libgurobi_c++ gurobi_c++
  HINTS ${GUROBI_ROOT}/lib
  PATHS /usr/lib
        ENV LIBRARY_PATH
        ENV LD_LIBRARY_PATH)

FIND_PATH(GUROBI_INCLUDE_DIR gurobi_c++.h ${GUROBI_ROOT}/include)
      
if( GUROBI_ROOT )
  SET(Gurobi_FOUND 1)
endif( GUROBI_ROOT )

SET(GUROBI_LIBRARIES ${GUROBI_LIB} ${GUROBI_LIB_C++})

#MARK_AS_ADVANCED(GUROBI_LIB GUROBI_LIB_C++ GUROBI_LIBRARIES)
