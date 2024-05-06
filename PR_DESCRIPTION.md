# Integrate ONNX Runtime into Openmind Project

## Description
This pull request integrates the ONNX Runtime into the Openmind project to enable ONNX model loading into a `Valuable` object. The following changes have been made:

- Edited `Valuable.h` and `Valuable.cpp` to include ONNX Runtime headers and implement methods `LoadONNXModel` and `RunONNXInference`.
- Updated `CMakeLists.txt` to directly include the `onnxruntimeTargets.cmake` file and add the `omnn/rt` directory to the include paths.
- Defined the `Valuable` target as a library in `CMakeLists.txt` and linked it to the `onnxruntime` library.
- Resolved redeclaration and 'does not name a type' errors in `Constant.cpp` and `ValuableDescendantContract.h`.
- Replaced `APPLE_CONSTEXPR` macro with `constexpr` in `Constant.h` and `Constant.cpp`.
- Removed `BOOST_CONSTEXPR` from default constructors and assignment operators in `Integer.h`.
- Replaced `NO_APPLE_CONSTEXPR` and `NO_CLANG_CONSTEXPR` macros with `constexpr` in `ValuableDescendantContract.h`.

## Testing
The integration has been built successfully using Ninja. However, no tests were found when running `ctest`. Further instructions are needed to proceed with testing the ONNX model integration or if there are specific tests to be run.

## Related Issues
- Resolves CMake configuration errors to ensure the ONNX Runtime library is correctly found and linked.
- Addresses compilation errors that arose during the build process.

## Notes
Please review the changes and provide feedback or further instructions on testing the ONNX model integration.

[This Devin run](https://preview.devin.ai/devin/90918ac8facb48c2b92e0809bc29dd57) was requested by Serg.
