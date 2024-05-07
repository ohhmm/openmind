# Install script for directory: /home/ubuntu/openmind/omnn/math

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/home/ubuntu/openmind/omnn/math/libmath.a")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES
    "/home/ubuntu/openmind/omnn/math/./Cache.h"
    "/home/ubuntu/openmind/omnn/math/./CompilerCompatibility.h"
    "/home/ubuntu/openmind/omnn/math/./Constant.h"
    "/home/ubuntu/openmind/omnn/math/./DuoValDescendant.h"
    "/home/ubuntu/openmind/omnn/math/./Exponentiation.h"
    "/home/ubuntu/openmind/omnn/math/./Formula.h"
    "/home/ubuntu/openmind/omnn/math/./FormulaOfVaWithSingleIntegerRoot.h"
    "/home/ubuntu/openmind/omnn/math/./Fraction.h"
    "/home/ubuntu/openmind/omnn/math/./Infinity.h"
    "/home/ubuntu/openmind/omnn/math/./Integer.h"
    "/home/ubuntu/openmind/omnn/math/./Logarithm.h"
    "/home/ubuntu/openmind/omnn/math/./Modulo.h"
    "/home/ubuntu/openmind/omnn/math/./OpenOps.h"
    "/home/ubuntu/openmind/omnn/math/./Polyfit.h"
    "/home/ubuntu/openmind/omnn/math/./PrincipalSurd.h"
    "/home/ubuntu/openmind/omnn/math/./Product.h"
    "/home/ubuntu/openmind/omnn/math/./SequenceOrderComparator.h"
    "/home/ubuntu/openmind/omnn/math/./Sum.h"
    "/home/ubuntu/openmind/omnn/math/./SySHA.h"
    "/home/ubuntu/openmind/omnn/math/./SymmetricDouble.h"
    "/home/ubuntu/openmind/omnn/math/./System.h"
    "/home/ubuntu/openmind/omnn/math/./Valuable.h"
    "/home/ubuntu/openmind/omnn/math/./ValuableCollectionDescendantContract.h"
    "/home/ubuntu/openmind/omnn/math/./ValuableDescendant.h"
    "/home/ubuntu/openmind/omnn/math/./ValuableDescendantContract.h"
    "/home/ubuntu/openmind/omnn/math/./VarHost.h"
    "/home/ubuntu/openmind/omnn/math/./Variable.h"
    "/home/ubuntu/openmind/omnn/math/./e.h"
    "/home/ubuntu/openmind/omnn/math/./i.h"
    "/home/ubuntu/openmind/omnn/math/./pi.h"
    )
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/ubuntu/openmind/omnn/math/test/cmake_install.cmake")
endif()

