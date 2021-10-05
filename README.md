# mGuard

## CMake build instruction

To configure a build directory, run
```
mkdir build
cd build
cmake ..
```

To run with test cases, run
```
mkdir build
cd build
cmake -DHAVE_TESTS=true ..
```

To build, run in the build directory:
```
make
```

To install, run after build:
```
make install
```