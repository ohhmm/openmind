# Define the source files for the extrapolator target
set(EXTRAPOLATOR_SOURCES
    Extrapolator.cpp
)

# Define the header files for the extrapolator target
set(EXTRAPOLATOR_HEADERS
    Extrapolator.h
)

# Add the extrapolator library target with the specified source files
add_library(extrapolator STATIC ${EXTRAPOLATOR_SOURCES} ${EXTRAPOLATOR_HEADERS})

# Set property to build with position independent code
set_property(TARGET extrapolator PROPERTY POSITION_INDEPENDENT_CODE ON)

# Include the directories where tasq.h and other headers are located
target_include_directories(extrapolator PUBLIC
    ../rt
    ../math
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Ensure the math library is built before the extrapolator target
add_dependencies(extrapolator math)

# Link the extrapolator library with its dependencies
target_link_libraries(extrapolator PUBLIC rt math)
