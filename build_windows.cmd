@echo off
setlocal enabledelayedexpansion

:: Set up vcpkg environment
if not defined VCPKG_ROOT (
    echo Error: VCPKG_ROOT environment variable is not set
    exit /b 1
)

:: Ensure vcpkg is bootstrapped
if not exist "%VCPKG_ROOT%\vcpkg.exe" (
    call "%VCPKG_ROOT%\bootstrap-vcpkg.bat"
)

:: Check for required tools
where ninja >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo Error: Ninja build system not found. Please install ninja-build.
    exit /b 1
)

:: Create build directory
if not exist build_windows mkdir build_windows
cd build_windows

:: Configure with CMake
cmake .. -G "Ninja Multi-Config" ^
    -DCMAKE_BUILD_TYPE=Release ^
    -DOPENMIND_BUILD_SAMPLES=OFF ^
    -DOPENMIND_BUILD_TESTS=ON ^
    -DOPENMIND_USE_OPENCL=ON ^
    -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain-windows.cmake ^
    -DVCPKG_TARGET_TRIPLET=x64-windows ^
    -DBoost_NO_SYSTEM_PATHS=ON ^
    -DBoost_NO_BOOST_CMAKE=OFF

if %ERRORLEVEL% neq 0 (
    echo CMake configuration failed
    exit /b 1
)

:: Build
cmake --build . --config Release

if %ERRORLEVEL% neq 0 (
    echo Build failed
    exit /b 1
)

echo Build completed successfully
exit /b 0
