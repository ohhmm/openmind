
set(deplist
    libskrypt
)

if(TARGET Boost::program_options)
    set(deplist ${deplist}
        Boost::filesystem
        Boost::program_options
        Boost::serialization
    )
elseif(Boost_PROGRAM_OPTIONS_LIBRARY)
    set(deplist ${deplist}
        ${Boost_FILESYSTEM_LIBRARY}
        ${Boost_PROGRAM_OPTIONS_LIBRARY}
        ${Boost_SERIALIZATION_LIBRARY}
    )
else()
    #set(deplist ${deplist} boost)
endif()

exe(${deplist})
