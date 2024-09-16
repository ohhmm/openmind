
function(prepend_toolchain_file)
    if(EXISTS ${CMAKE_TOOLCHAIN_FILE})
        list(PREPEND CMAKE_PROJECT_TOP_LEVEL_INCLUDES "${CMAKE_TOOLCHAIN_FILE}")
        set(${CMAKE_PROJECT_TOP_LEVEL_INCLUDES} "${${CMAKE_PROJECT_TOP_LEVEL_INCLUDES}}" PARENT_SCOPE)
        list(PREPEND CMAKE_TRY_COMPILE_PLATFORM_VARIABLES CMAKE_PROJECT_TOP_LEVEL_INCLUDES)
        set(${CMAKE_TRY_COMPILE_PLATFORM_VARIABLES} "${${CMAKE_TRY_COMPILE_PLATFORM_VARIABLES}}" PARENT_SCOPE)
    endif()
endfunction()

macro(find_vcpkg)
    if(NOT EXISTS ${CMAKE_TOOLCHAIN_FILE})
        if(NOT EXISTS ${VCPKG_EXECUTABLE})
            find_program(VCPKG_EXECUTABLE vcpkg PATHS
                "$ENV{VCPKG_ROOT}"
                "${VCPKG_ROOT}"
                "$ENV{ProgramFiles}/Microsoft Visual Studio/*/*/VC/vcpkg/"
                "$ENV{USERPROFILE}$ENV{HOME}/vcpkg/"
                "$ENV{USERPROFILE}$ENV{HOME}/.vcpkg-clion/"
            )
        endif()
        if (EXISTS ${VCPKG_EXECUTABLE})
            message("VCPKG_EXECUTABLE=${VCPKG_EXECUTABLE}")
            get_filename_component(VCPKG_ROOT ${VCPKG_EXECUTABLE} DIRECTORY)
            set(VCPKG_ROOT "${VCPKG_ROOT}" CACHE PATH "Path to vcpkg installation" FORCE)
            set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file" FORCE)
        endif()
    endif()

    if(EXISTS ${CMAKE_TOOLCHAIN_FILE})
        set(VCPKG_FOUND ON CACHE BOOL "VCPKG toolchain file found ${CMAKE_TOOLCHAIN_FILE}" FORCE)
        #prepend_toolchain_file()
    else()
        set(VCPKG_FOUND NO CACHE BOOL "VCPKG not found" FORCE)
    endif()
endmacro()

macro(fetch_vcpkg)
    include(gitect)
    include(FetchContent)
    FetchContent_Populate(	VCPKG
        GIT_REPOSITORY https://github.com/microsoft/vcpkg.git
        GIT_TAG        2024.08.23
        SOURCE_DIR     "$ENV{USERPROFILE}$ENV{HOME}/vcpkg"
    )

    if(GIT_EXECUTABLE)
        set(VCPKG_ROOT "$ENV{USERPROFILE}$ENV{HOME}/vcpkg" CACHE PATH "Path to vcpkg installation")
        if(EXISTS ${VCPKG_ROOT})
        # Ensure we are on a branch before pulling
            execute_process(
                COMMAND ${GIT_EXECUTABLE} -C ${VCPKG_ROOT} checkout master
                RESULT_VARIABLE GIT_CHECKOUT_RESULT
            )
            if(NOT GIT_CHECKOUT_RESULT EQUAL 0)
                message(FATAL_ERROR "Failed to checkout master branch in vcpkg repository")
            endif()

            # Pull the latest changes
            execute_process(
                COMMAND ${GIT_EXECUTABLE} -C ${VCPKG_ROOT} pull
                RESULT_VARIABLE GIT_PULL_RESULT
            )
            if(NOT GIT_PULL_RESULT EQUAL 0)
                message(FATAL_ERROR "Failed to pull latest changes from vcpkg repository")
            endif()
        else()
            # Clone the repository if it doesn't exist
            execute_process(
                COMMAND ${GIT_EXECUTABLE} clone https://github.com/microsoft/vcpkg ${VCPKG_ROOT}
                RESULT_VARIABLE GIT_RESULT
            )
            if(NOT GIT_RESULT EQUAL 0)
                message(WARNING "Failed to clone vcpkg repository")
            endif()
        endif()
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E chdir ${VCPKG_ROOT} ${VCPKG_ROOT}/bootstrap-vcpkg.sh
            RESULT_VARIABLE BOOTSTRAP_RESULT
        )
        if(NOT BOOTSTRAP_RESULT EQUAL 0)
            message(WARNING "Failed to bootstrap vcpkg")
        endif()
        set(VCPKG_EXECUTABLE "${VCPKG_ROOT}/vcpkg" CACHE FILEPATH "Path to vcpkg executable" FORCE)
        set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file" FORCE)
        set(VCPKG_FOUND ON CACHE BOOL "VCPKG found" FORCE)
    endif()
endmacro()

if(NOT VCPKG_FOUND)
    find_vcpkg()
else()
    #prepend_toolchain_file()
endif()

option(OPENMIND_USE_VCPKG "Use vcpkg" ${VCPKG_FOUND})

if(OPENMIND_USE_VCPKG)
    if(NOT VCPKG_FOUND)
        find_vcpkg()
        if(NOT VCPKG_FOUND)
            fetch_vcpkg()
            if(NOT VCPKG_FOUND)
                find_vcpkg()
            endif()
        endif()
    endif()
    if(VCPKG_FOUND)
        if(_IS_DEBUG)
            option(VCPKG_PREFER_SYSTEM_LIBS "Make Vcpkg prefer system libraries" ON)
        endif()
    else()
        message(WARNING "Vcpkg not found")
    endif()
endif()
