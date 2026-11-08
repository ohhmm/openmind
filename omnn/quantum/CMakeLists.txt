find_optional_pkg(OpenSSL)
if(OPENMIND_USE_OpenSSL)
    lib(math extrapolator OpenMind OpenSSL::SSL OpenSSL::Crypto)
endif()
