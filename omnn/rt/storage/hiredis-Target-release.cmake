# Avoid multiple calls to find_package to append duplicated properties to the targets
include_guard()########### VARIABLES #######################################################################
#############################################################################################
set(hiredis_FRAMEWORKS_FOUND_RELEASE "") # Will be filled later
conan_find_apple_frameworks(hiredis_FRAMEWORKS_FOUND_RELEASE "${hiredis_FRAMEWORKS_RELEASE}" "${hiredis_FRAMEWORK_DIRS_RELEASE}")

set(hiredis_LIBRARIES_TARGETS "") # Will be filled later


######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
if(NOT TARGET hiredis_DEPS_TARGET)
    add_library(hiredis_DEPS_TARGET INTERFACE IMPORTED)
endif()

set_property(TARGET hiredis_DEPS_TARGET
             APPEND PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Release>:${hiredis_FRAMEWORKS_FOUND_RELEASE}>
             $<$<CONFIG:Release>:${hiredis_SYSTEM_LIBS_RELEASE}>
             $<$<CONFIG:Release>:>)

####### Find the libraries declared in cpp_info.libs, create an IMPORTED target for each one and link the
####### hiredis_DEPS_TARGET to all of them
conan_package_library_targets("${hiredis_LIBS_RELEASE}"    # libraries
                              "${hiredis_LIB_DIRS_RELEASE}" # package_libdir
                              "${hiredis_BIN_DIRS_RELEASE}" # package_bindir
                              "${hiredis_LIBRARY_TYPE_RELEASE}"
                              "${hiredis_IS_HOST_WINDOWS_RELEASE}"
                              hiredis_DEPS_TARGET
                              hiredis_LIBRARIES_TARGETS  # out_libraries_targets
                              "_RELEASE"
                              "hiredis"    # package_name
                              "${hiredis_NO_SONAME_MODE_RELEASE}")  # soname

# FIXME: What is the result of this for multi-config? All configs adding themselves to path?
set(CMAKE_MODULE_PATH ${hiredis_BUILD_DIRS_RELEASE} ${CMAKE_MODULE_PATH})

########## COMPONENTS TARGET PROPERTIES Release ########################################

    ########## COMPONENT hiredis::hiredis #############

        set(hiredis_hiredis_hiredis_FRAMEWORKS_FOUND_RELEASE "")
        conan_find_apple_frameworks(hiredis_hiredis_hiredis_FRAMEWORKS_FOUND_RELEASE "${hiredis_hiredis_hiredis_FRAMEWORKS_RELEASE}" "${hiredis_hiredis_hiredis_FRAMEWORK_DIRS_RELEASE}")

        set(hiredis_hiredis_hiredis_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET hiredis_hiredis_hiredis_DEPS_TARGET)
            add_library(hiredis_hiredis_hiredis_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET hiredis_hiredis_hiredis_DEPS_TARGET
                     APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Release>:${hiredis_hiredis_hiredis_FRAMEWORKS_FOUND_RELEASE}>
                     $<$<CONFIG:Release>:${hiredis_hiredis_hiredis_SYSTEM_LIBS_RELEASE}>
                     $<$<CONFIG:Release>:${hiredis_hiredis_hiredis_DEPENDENCIES_RELEASE}>
                     )

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'hiredis_hiredis_hiredis_DEPS_TARGET' to all of them
        conan_package_library_targets("${hiredis_hiredis_hiredis_LIBS_RELEASE}"
                              "${hiredis_hiredis_hiredis_LIB_DIRS_RELEASE}"
                              "${hiredis_hiredis_hiredis_BIN_DIRS_RELEASE}" # package_bindir
                              "${hiredis_hiredis_hiredis_LIBRARY_TYPE_RELEASE}"
                              "${hiredis_hiredis_hiredis_IS_HOST_WINDOWS_RELEASE}"
                              hiredis_hiredis_hiredis_DEPS_TARGET
                              hiredis_hiredis_hiredis_LIBRARIES_TARGETS
                              "_RELEASE"
                              "hiredis_hiredis_hiredis"
                              "${hiredis_hiredis_hiredis_NO_SONAME_MODE_RELEASE}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET hiredis::hiredis
                     APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Release>:${hiredis_hiredis_hiredis_OBJECTS_RELEASE}>
                     $<$<CONFIG:Release>:${hiredis_hiredis_hiredis_LIBRARIES_TARGETS}>
                     )

        if("${hiredis_hiredis_hiredis_LIBS_RELEASE}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET hiredis::hiredis
                         APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                         hiredis_hiredis_hiredis_DEPS_TARGET)
        endif()

        set_property(TARGET hiredis::hiredis APPEND PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Release>:${hiredis_hiredis_hiredis_LINKER_FLAGS_RELEASE}>)
        set_property(TARGET hiredis::hiredis APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Release>:${hiredis_hiredis_hiredis_INCLUDE_DIRS_RELEASE}>)
        set_property(TARGET hiredis::hiredis APPEND PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Release>:${hiredis_hiredis_hiredis_LIB_DIRS_RELEASE}>)
        set_property(TARGET hiredis::hiredis APPEND PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Release>:${hiredis_hiredis_hiredis_COMPILE_DEFINITIONS_RELEASE}>)
        set_property(TARGET hiredis::hiredis APPEND PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Release>:${hiredis_hiredis_hiredis_COMPILE_OPTIONS_RELEASE}>)

    ########## AGGREGATED GLOBAL TARGET WITH THE COMPONENTS #####################
    set_property(TARGET hiredis::hiredis APPEND PROPERTY INTERFACE_LINK_LIBRARIES hiredis::hiredis)

########## For the modules (FindXXX)
set(hiredis_LIBRARIES_RELEASE hiredis::hiredis)
