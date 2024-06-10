# CMake configuration for the ImageCodec_test

# Find Boost libraries
find_package(Boost COMPONENTS unit_test_framework log log_setup REQUIRED)

# Add the test executable
add_executable(ImageCodec_test image_codec_test.cpp)

# Link necessary libraries
target_link_libraries(ImageCodec_test PRIVATE rt Boost::unit_test_framework math_lib storage Boost::log Boost::log_setup extrapolator)

# Define test source and binary directories
target_compile_definitions(ImageCodec_test PRIVATE
    -DTEST_SRC_DIR="${CMAKE_CURRENT_SOURCE_DIR}/"
    -DTEST_BIN_DIR="${CMAKE_CURRENT_BINARY_DIR}/"
)

# Add the test to CTest
add_test(NAME ImageCodec_test COMMAND ImageCodec_test)
