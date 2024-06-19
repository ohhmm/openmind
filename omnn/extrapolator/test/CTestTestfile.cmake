# CMake generated Testfile for 
# Source directory: /home/ubuntu/openmind/omnn/extrapolator/test
# Build directory: /home/ubuntu/openmind/omnn/extrapolator/test
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
if(CTEST_CONFIGURATION_TYPE MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  add_test(image_codec_test "/home/ubuntu/openmind/bin/Debug/image_codec_test")
  set_tests_properties(image_codec_test PROPERTIES  WORKING_DIRECTORY "/home/ubuntu/openmind/omnn/extrapolator/test/image_codec_test.db" _BACKTRACE_TRIPLES "/home/ubuntu/openmind/cmake/bins.cmake;308;add_test;/home/ubuntu/openmind/omnn/extrapolator/test/CMakeLists.txt;1;test;/home/ubuntu/openmind/omnn/extrapolator/test/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  add_test(image_codec_test "/home/ubuntu/openmind/bin/Release/image_codec_test")
  set_tests_properties(image_codec_test PROPERTIES  WORKING_DIRECTORY "/home/ubuntu/openmind/omnn/extrapolator/test/image_codec_test.db" _BACKTRACE_TRIPLES "/home/ubuntu/openmind/cmake/bins.cmake;308;add_test;/home/ubuntu/openmind/omnn/extrapolator/test/CMakeLists.txt;1;test;/home/ubuntu/openmind/omnn/extrapolator/test/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
  add_test(image_codec_test "/home/ubuntu/openmind/bin/RelWithDebInfo/image_codec_test")
  set_tests_properties(image_codec_test PROPERTIES  WORKING_DIRECTORY "/home/ubuntu/openmind/omnn/extrapolator/test/image_codec_test.db" _BACKTRACE_TRIPLES "/home/ubuntu/openmind/cmake/bins.cmake;308;add_test;/home/ubuntu/openmind/omnn/extrapolator/test/CMakeLists.txt;1;test;/home/ubuntu/openmind/omnn/extrapolator/test/CMakeLists.txt;0;")
else()
  add_test(image_codec_test NOT_AVAILABLE)
endif()
if(CTEST_CONFIGURATION_TYPE MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  add_test(test_extrapolator "/home/ubuntu/openmind/bin/Debug/test_extrapolator")
  set_tests_properties(test_extrapolator PROPERTIES  WORKING_DIRECTORY "/home/ubuntu/openmind/omnn/extrapolator/test/test_extrapolator.db" _BACKTRACE_TRIPLES "/home/ubuntu/openmind/cmake/bins.cmake;308;add_test;/home/ubuntu/openmind/omnn/extrapolator/test/CMakeLists.txt;1;test;/home/ubuntu/openmind/omnn/extrapolator/test/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  add_test(test_extrapolator "/home/ubuntu/openmind/bin/Release/test_extrapolator")
  set_tests_properties(test_extrapolator PROPERTIES  WORKING_DIRECTORY "/home/ubuntu/openmind/omnn/extrapolator/test/test_extrapolator.db" _BACKTRACE_TRIPLES "/home/ubuntu/openmind/cmake/bins.cmake;308;add_test;/home/ubuntu/openmind/omnn/extrapolator/test/CMakeLists.txt;1;test;/home/ubuntu/openmind/omnn/extrapolator/test/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
  add_test(test_extrapolator "/home/ubuntu/openmind/bin/RelWithDebInfo/test_extrapolator")
  set_tests_properties(test_extrapolator PROPERTIES  WORKING_DIRECTORY "/home/ubuntu/openmind/omnn/extrapolator/test/test_extrapolator.db" _BACKTRACE_TRIPLES "/home/ubuntu/openmind/cmake/bins.cmake;308;add_test;/home/ubuntu/openmind/omnn/extrapolator/test/CMakeLists.txt;1;test;/home/ubuntu/openmind/omnn/extrapolator/test/CMakeLists.txt;0;")
else()
  add_test(test_extrapolator NOT_AVAILABLE)
endif()
