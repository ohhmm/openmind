# CMake generated Testfile for 
# Source directory: /home/ubuntu/openmind/omnn/extrapolator/test
# Build directory: /home/ubuntu/openmind/omnn/extrapolator/test
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(image_codec_test "/home/ubuntu/openmind/bin/image_codec_test")
set_tests_properties(image_codec_test PROPERTIES  WORKING_DIRECTORY "/home/ubuntu/openmind/omnn/extrapolator/test/image_codec_test.db" _BACKTRACE_TRIPLES "/home/ubuntu/openmind/cmake/bins.cmake;308;add_test;/home/ubuntu/openmind/omnn/extrapolator/test/CMakeLists.txt;1;test;/home/ubuntu/openmind/omnn/extrapolator/test/CMakeLists.txt;0;")
add_test(test_extrapolator "/home/ubuntu/openmind/bin/test_extrapolator")
set_tests_properties(test_extrapolator PROPERTIES  WORKING_DIRECTORY "/home/ubuntu/openmind/omnn/extrapolator/test/test_extrapolator.db" _BACKTRACE_TRIPLES "/home/ubuntu/openmind/cmake/bins.cmake;308;add_test;/home/ubuntu/openmind/omnn/extrapolator/test/CMakeLists.txt;1;test;/home/ubuntu/openmind/omnn/extrapolator/test/CMakeLists.txt;0;")
