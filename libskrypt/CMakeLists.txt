
set(deplist math)

if(TARGET Boost::dll)
	set(deplist ${deplist} Boost::dll)
elseif(Boost_DLL_LIBRARY)
	set(deplist ${deplist} ${Boost_DLL_LIBRARY})
else()
	set(deplist ${deplist} boost)
endif()

lib(${deplist})
