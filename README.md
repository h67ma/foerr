# External libraries
This project uses the following libraries:
* [SFML](https://www.sfml-dev.org), which is licensed under the [zlib/png license](https://www.sfml-dev.org/license.php)
* [JSON for Modern C++](https://github.com/nlohmann/json),
which is licensed under the [MIT license](https://github.com/nlohmann/json/blob/develop/LICENSE.MIT)

# Build
Refer to [SFML documentation](https://www.sfml-dev.org/tutorials/2.6/compile-with-cmake.php#installing-dependencies)
for satisfying SFML dependencies.

```
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build . -j`nproc` --config Debug
```

Source code of dependencies is automaticaly downloaded on project configure. Libraries are built from source and linked statically, with the exception of openal32.

## Switching build type
```
cmake -DCMAKE_BUILD_TYPE=Release ..
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
