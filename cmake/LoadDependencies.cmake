include(ExternalProject) # needed to use "externalproject_.." commands
set_property(DIRECTORY PROPERTY EP_BASE Dependencies) # set the base name for the EP=ExternalProject directory

SET(DEPENDENCIES)

list(APPEND DEPENDENCIES TCLAP_Project)
ExternalProject_ADD(
   TCLAP_Project
   URL "http://downloads.sourceforge.net/project/tclap/tclap-1.2.1.tar.gz"
   # or http://kent.dl.sourceforge.net/project/tclap/tclap-1.2.1.tar.gz
   INSTALL_COMMAND ""
   BUILD_COMMAND ""
   CONFIGURE_COMMAND ""
   )

 list(APPEND DEPENDENCIES Meta_Project)
 ExternalProject_Add(
    Meta_Project
    GIT_REPOSITORY "https://github.com/ericniebler/meta.git"
    GIT_TAG "master"
    INSTALL_COMMAND ""
    BUILD_COMMAND ""
    CONFIGURE_COMMAND ""
    )

if(DOWNLOAD_LIBPQXX)
list(APPEND DEPENDENCIES PostgreSql_Project)
ExternalProject_Add(
   PostgreSql_Project
   GIT_REPOSITORY "https://github.com/jtv/libpqxx"
   GIT_TAG "master"
   INSTALL_COMMAND ""
   BUILD_COMMAND ""
   CONFIGURE_COMMAND ""
   )
endif()

list(APPEND DEPENDENCIES CATCH_Project)
ExternalProject_Add(
   CATCH_Project
   GIT_REPOSITORY "https://github.com/philsquared/Catch"
   GIT_TAG "master"
   INSTALL_COMMAND ""
   BUILD_COMMAND ""
   CONFIGURE_COMMAND ""
   )

list(APPEND DEPENDENCIES PYBIND11_Project)
ExternalProject_Add(
   PYBIND11_Project
   GIT_REPOSITORY "https://github.com/pybind/pybind11"
   GIT_TAG "master"
   INSTALL_COMMAND ""
   BUILD_COMMAND ""
   CONFIGURE_COMMAND ""
   )
   
list(APPEND DEPENDENCIES PEGTL_Project)
ExternalProject_Add(
   PEGTL_Project
   GIT_REPOSITORY "https://github.com/ColinH/PEGTL.git"
   GIT_TAG "2.1.2"
   INSTALL_COMMAND ""
   BUILD_COMMAND ""
   CONFIGURE_COMMAND ""
   )
ExternalProject_Get_Property(PEGTL_Project install_dir)

ExternalProject_Add (EKP
   DEPENDS ${DEPENDENCIES}
   SOURCE_DIR ${PROJECT_SOURCE_DIR}
   CMAKE_ARGS -DDOWNLOAD_DEPENDENCIES=OFF -DDOWNLOAD_LIBPQXX=OFF
   INSTALL_COMMAND ""
   BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})

# ExternalProject_Get_Property(TCLAP_Project install_dir)
# message(STATUS "install dir:  ${install_dir}")
