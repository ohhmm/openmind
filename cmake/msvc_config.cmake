cmake
if(MSVC)
    # Match Boost's iterator debug level for compatibility
    add_definitions(-D_ITERATOR_DEBUG_LEVEL=2)
    set(Boost_USE_DEBUG_RUNTIME ON)
    set(Boost_USE_STATIC_RUNTIME OFF)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /D_ITERATOR_DEBUG_LEVEL=2")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /D_ITERATOR_DEBUG_LEVEL=2")
endif()
