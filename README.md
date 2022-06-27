# Build
```
mkdir build
cd build
cmake ../src/game
cmake --build . -j`nproc`
```

## Verbose build
Just add `-v`.

# Run
```
cd $PROJECT_ROOT
build/foerr
```

`$PROJECT_ROOT` is a directory containing the `res` directory.

Instead of `cmake ..`, run `cmake -DCMAKE_BUILD_TYPE=Release ..`.

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
