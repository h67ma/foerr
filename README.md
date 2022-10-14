# Dependencies

* [SFML](https://www.sfml-dev.org)
* [JSON for Modern C++](https://github.com/nlohmann/json)

# Build
```
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build . -j`nproc` --config Debug
```

Source code of dependencies is automaticaly downloaded on project configure. Libraries are built from source and linked statically, with the exception of openal32.

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
build/bin/Release/foerr
```
Where `$PROJECT_ROOT` is a directory containing the `res` directory.

On Windows, `openal32.dll` must be present in PATH or in current directory.

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
