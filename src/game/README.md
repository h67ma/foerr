# Build
```
cd src/game
mkdir build
cd build
cmake ..
cmake --build . -j`nproc`
```

Note: executable will be placed in repo root dir.

## Release build
Instead of `cmake ..`, run `cmake -DCMAKE_BUILD_TYPE=Release ..`.

## Verbose build
Just add `-v`.

# Clean
```
cd build
cmake --build . --target clean
```

... or just delete the build dir.
