find_optional_pkg(OpenSSL)
if(OpenSSL_FOUND)
    lib(math OpenSSL::SSL OpenSSL::Crypto)
endif()
