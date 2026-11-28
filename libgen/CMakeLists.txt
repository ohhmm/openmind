
if(MSVC)
	option(PNG_SHARED "Build shared lib" OFF)
endif()
option(PNG_STATIC "Build static lib" ON)

set(deplist
	libskrypt
	Boost::compute
    Boost::filesystem
    Boost::program_options
    Boost::serialization
	)
# fs_io now writes PNG images using Boost.GIL's PNG backend.
find_package(PNG QUIET)
if(TARGET PNG::PNG)
	set(deplist ${deplist} PNG::PNG)
elseif(PNG_LIBRARIES)
	set(deplist ${deplist} ${PNG_LIBRARIES})
endif()
if(TARGET Boost::program_options)
	set(deplist ${deplist} Boost::program_options)
elseif(Boost_PROGRAM_OPTIONS_LIBRARY)
	set(deplist ${deplist} ${Boost_PROGRAM_OPTIONS_LIBRARY})
else()
	set(deplist ${deplist} boost)
endif()

lib(${deplist})

