if(BUILD_TESTS)
  enable_testing()
  
  add_executable(test_enumeration test_enumeration.cpp)

  add_executable(EKPObject EKP_Object_test.cxx)

  #target_sources(test_main PUBLIC temptest.cxx)

  #if(WITH_SQLITE3)
  #  target_compile_definitions(test_main PUBLIC -DWITH_SQLITE3)
  #  target_link_libraries(test_main ${SQLITE3_LIB})
  #endif()
  #if(WITH_LIBPQXX)
  #  target_compile_definitions(test_main PUBLIC -DWITH_LIBPQXX)
  #  target_link_libraries(test_main ${PQ_LIB} ${PQXX_LIB})
  #endif()
  #target_link_libraries(test_main ${HDF5_LIBRARIES})

  #add_custom_command(TARGET test_main PRE_BUILD
  #COMMAND cp ${CMAKE_SOURCE_DIR}/src/unittest/ParserTest.txt $<TARGET_FILE_DIR:test_main>)
  #add_custom_command(TARGET test_main PRE_BUILD
  #COMMAND cp ${CMAKE_SOURCE_DIR}/src/unittest/ParserFunctionTest.txt $<TARGET_FILE_DIR:test_main>)
  #add_custom_command(TARGET test_main PRE_BUILD
  #COMMAND cp ${CMAKE_SOURCE_DIR}/src/unittest/FunctionTest.txt $<TARGET_FILE_DIR:test_main>)

  #add_test(NAME MAIN_TEST COMMAND test_main)

endif()
