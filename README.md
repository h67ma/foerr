# Dependencies
* [SFML library](https://www.sfml-dev.org/download/sfml/2.5.1/)

# Build
```
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ../src/game
cmake --build . -j`nproc` --config Debug
```
In case SFML is installed in non-standard path (or when using Windows), set the following environment variables:
```
CMAKE_INCLUDE_PATH=path/to/SFML-x.y.z/include
CMAKE_LIBRARY_PATH=path/to/SFML-x.y.z/lib
```

## Switching build type
```
cmake -DCMAKE_BUILD_TYPE=Release ../src/game
cmake --build . -j`nproc` --config Release
```

## Verbose build
Just add `-v`.

# Run
```
cd $PROJECT_ROOT
build/foerr
```
Where `$PROJECT_ROOT` is a directory containing the `res` directory.

SFML bin dir path must be present in PATH, or libraries must be present in the same dir as game executable.

# Clean
```
cd build
cmake --build . --target clean
```

...or just delete the `build` dir.

# Install
```
cd build
cmake --install . --config Release --prefix $PATH_TO_INSTALL
```
