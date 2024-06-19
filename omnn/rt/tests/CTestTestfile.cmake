# CMake generated Testfile for 
# Source directory: /home/ubuntu/openmind/omnn/rt/tests
# Build directory: /home/ubuntu/openmind/omnn/rt/tests
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
if(CTEST_CONFIGURATION_TYPE MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  add_test(fastest_get "/home/ubuntu/openmind/bin/Debug/fastest_get")
  set_tests_properties(fastest_get PROPERTIES  WORKING_DIRECTORY "/home/ubuntu/openmind/omnn/rt/tests/fastest_get.db" _BACKTRACE_TRIPLES "/home/ubuntu/openmind/cmake/bins.cmake;308;add_test;/home/ubuntu/openmind/omnn/rt/tests/CMakeLists.txt;1;test;/home/ubuntu/openmind/omnn/rt/tests/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  add_test(fastest_get "/home/ubuntu/openmind/bin/Release/fastest_get")
  set_tests_properties(fastest_get PROPERTIES  WORKING_DIRECTORY "/home/ubuntu/openmind/omnn/rt/tests/fastest_get.db" _BACKTRACE_TRIPLES "/home/ubuntu/openmind/cmake/bins.cmake;308;add_test;/home/ubuntu/openmind/omnn/rt/tests/CMakeLists.txt;1;test;/home/ubuntu/openmind/omnn/rt/tests/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
  add_test(fastest_get "/home/ubuntu/openmind/bin/RelWithDebInfo/fastest_get")
  set_tests_properties(fastest_get PROPERTIES  WORKING_DIRECTORY "/home/ubuntu/openmind/omnn/rt/tests/fastest_get.db" _BACKTRACE_TRIPLES "/home/ubuntu/openmind/cmake/bins.cmake;308;add_test;/home/ubuntu/openmind/omnn/rt/tests/CMakeLists.txt;1;test;/home/ubuntu/openmind/omnn/rt/tests/CMakeLists.txt;0;")
else()
  add_test(fastest_get NOT_AVAILABLE)
endif()
