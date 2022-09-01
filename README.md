# Dependencies

## SFML
[SFML](https://www.sfml-dev.org)

SFML provides prebuilt binaries. Get them [here](https://www.sfml-dev.org/download/sfml/2.5.1/).

## jsoncpp
[jsoncpp](https://github.com/open-source-parsers/jsoncpp)

Building on Windows:
```
mkdir build
cd build
cmake -DBUILD_STATIC_LIBS=OFF -DBUILD_SHARED_LIBS=ON ..
cmake --build . --config Release
cmake --build . --config Debug
move lib\Release\jsoncpp.lib lib\jsoncpp.lib
move lib\Debug\jsoncpp.lib lib\jsoncpp-d.lib
```

# Build
```
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ../src/game
cmake --build . -j`nproc` --config Debug
```
In case libraries are installed in non-standard path (or when using Windows), set the following environment variables:
```
CMAKE_INCLUDE_PATH=path/to/SFML/include;path/to/jsoncpp/include
CMAKE_LIBRARY_PATH=path/to/SFML/lib;path/to/jsoncpp/build/lib
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

Game requires shared libraries (see *Dependencies*). Libraries bin dirs paths must be present in PATH, or libraries must be present in curent directory (Windows).

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
