../U-00DC-Sprache/build-release-msvc-ninja/llvm/bin/clang.exe -cc1 -triple spir test.cl -O0 -emit-llvm-bc -o test.bc -cl-std=clc++ -finclude-default-header
../SPIRV-LLVM-Translator/build-debug/tools/llvm-spirv/llvm-spirv.exe test.bc -o test.sprv
