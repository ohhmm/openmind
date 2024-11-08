# Force Boost dynamic linking configuration
set(Boost_USE_STATIC_LIBS OFF CACHE BOOL "Use dynamic Boost libraries" FORCE)
set(Boost_USE_MULTITHREADED ON CACHE BOOL "Use multithreaded Boost libraries" FORCE)
set(Boost_USE_STATIC_RUNTIME OFF CACHE BOOL "Use dynamic runtime" FORCE)
add_definitions(-DBOOST_ALL_DYN_LINK)

# Remove any static linking definitions that might conflict
remove_definitions(-DBOOST_ALL_STATIC_LINK)
