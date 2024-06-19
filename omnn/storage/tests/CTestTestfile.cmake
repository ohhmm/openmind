# CMake generated Testfile for 
# Source directory: /home/ubuntu/openmind/omnn/storage/tests
# Build directory: /home/ubuntu/openmind/omnn/storage/tests
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
if(CTEST_CONFIGURATION_TYPE MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  add_test(LevelDbStorage "/home/ubuntu/openmind/bin/Debug/LevelDbStorage")
  set_tests_properties(LevelDbStorage PROPERTIES  WORKING_DIRECTORY "/home/ubuntu/openmind/omnn/storage/tests/LevelDbStorage.db" _BACKTRACE_TRIPLES "/home/ubuntu/openmind/cmake/bins.cmake;308;add_test;/home/ubuntu/openmind/omnn/storage/tests/CMakeLists.txt;1;test;/home/ubuntu/openmind/omnn/storage/tests/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  add_test(LevelDbStorage "/home/ubuntu/openmind/bin/Release/LevelDbStorage")
  set_tests_properties(LevelDbStorage PROPERTIES  WORKING_DIRECTORY "/home/ubuntu/openmind/omnn/storage/tests/LevelDbStorage.db" _BACKTRACE_TRIPLES "/home/ubuntu/openmind/cmake/bins.cmake;308;add_test;/home/ubuntu/openmind/omnn/storage/tests/CMakeLists.txt;1;test;/home/ubuntu/openmind/omnn/storage/tests/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
  add_test(LevelDbStorage "/home/ubuntu/openmind/bin/RelWithDebInfo/LevelDbStorage")
  set_tests_properties(LevelDbStorage PROPERTIES  WORKING_DIRECTORY "/home/ubuntu/openmind/omnn/storage/tests/LevelDbStorage.db" _BACKTRACE_TRIPLES "/home/ubuntu/openmind/cmake/bins.cmake;308;add_test;/home/ubuntu/openmind/omnn/storage/tests/CMakeLists.txt;1;test;/home/ubuntu/openmind/omnn/storage/tests/CMakeLists.txt;0;")
else()
  add_test(LevelDbStorage NOT_AVAILABLE)
endif()
