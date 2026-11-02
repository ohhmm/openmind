find_package(Boost REQUIRED COMPONENTS program_options)


if(MSVC)
	option(PNG_SHARED "Build shared lib" OFF)
endif()
option(PNG_STATIC "Build static lib" ON)

set(deplist 
	libskrypt

    Boost::filesystem
    Boost::program_options
    Boost::serialization
	)
# fs_io no longer requires image-encoding dependencies.
if(TARGET Boost::program_options)
	set(deplist ${deplist} Boost::program_options)
elseif(Boost_PROGRAM_OPTIONS_LIBRARY)
	set(deplist ${deplist} ${Boost_PROGRAM_OPTIONS_LIBRARY})
else()
	set(deplist ${deplist} boost)
endif()

lib(${deplist})

