SET(ARMADILLO_FOUND 0)

FIND_LIBRARY(ARMADILLO_LIB
  NAMES libarmadillo armadillo
  PATH_SUFFIXES build)
FIND_LIBRARY(LAPACK_LIB
  NAMES liblapack lapack)
FIND_LIBRARY(OPENBLAS_LIB
  NAMES libopenblas openblas)

FIND_PATH(ARMADILLO_INCLUDE armadillo)

if( ARMADILLO_LIB )
  SET(ARMADILLO_FOUND 1)
endif( ARMADILLO_LIB )
