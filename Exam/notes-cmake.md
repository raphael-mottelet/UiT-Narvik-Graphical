
# File i use for runing my program and compile all .cpp and .h files

# Only for me because my PC have some admin access issue

Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope Process    

# Optional but i use it anyway
if (Test-Path .\shadeers_embedded.h) { Rename-Item .\shadeers_embedded.h shaders_embedded.h }

# Ensure MinGW is on PATH
$env:Path = "C:\WINLIBS-I686-POSIX-DWARF-GCC-15.2.0-MINGW-W64UCRT-13.0.0-R2\mingw32\bin;$env:Path"

# Clean build
Remove-Item -Recurse -Force .\build -ErrorAction SilentlyContinue
mkdir build | Out-Null

# Configure
& "C:\Program Files\CMake\bin\cmake.exe" -S . -B build -G "MinGW Makefiles" `
  -DCMAKE_BUILD_TYPE=Release `
  -DCMAKE_SH=CMAKE_SH-NOTFOUND `
  -DCMAKE_MAKE_PROGRAM="C:\WINLIBS-I686-POSIX-DWARF-GCC-15.2.0-MINGW-W64UCRT-13.0.0-R2\mingw32\bin\mingw32-make.exe" `
  -DCMAKE_C_COMPILER="C:\WINLIBS-I686-POSIX-DWARF-GCC-15.2.0-MINGW-W64UCRT-13.0.0-R2\mingw32\bin\gcc.exe" `
  -DCMAKE_CXX_COMPILER="C:\WINLIBS-I686-POSIX-DWARF-GCC-15.2.0-MINGW-W64UCRT-13.0.0-R2\mingw32\bin\g++.exe"

# Build
& "C:\Program Files\CMake\bin\cmake.exe" --build build --target exam --parallel

# Run
.\build\exam.exe

