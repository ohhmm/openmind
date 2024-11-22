# Load the debug and release variables
file(GLOB DATA_FILES "${CMAKE_CURRENT_LIST_DIR}/hiredis-*-data.cmake")

foreach(f ${DATA_FILES})
    include(${f})
endforeach()

# Create the targets for all the components
foreach(_COMPONENT ${hiredis_COMPONENT_NAMES} )
    if(NOT TARGET ${_COMPONENT})
        add_library(${_COMPONENT} INTERFACE IMPORTED)
        message(${hiredis_MESSAGE_MODE} "Conan: Component target declared '${_COMPONENT}'")
    endif()
endforeach()

if(NOT TARGET hiredis::hiredis)
    add_library(hiredis::hiredis INTERFACE IMPORTED)
    message(${hiredis_MESSAGE_MODE} "Conan: Target declared 'hiredis::hiredis'")
endif()
# Load the debug and release library finders
file(GLOB CONFIG_FILES "${CMAKE_CURRENT_LIST_DIR}/hiredis-Target-*.cmake")

foreach(f ${CONFIG_FILES})
    include(${f})
endforeach()