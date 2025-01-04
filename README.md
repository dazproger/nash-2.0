# Nash equilibrium research project

This is a repository for a research project on Game theory, specifically concentrated on finding games without Nash equilibrium.


## Requirements

* Clang/GCC (with address sanitizer and undefined behaviour sanitizer)

* CMake version >= 3.26

* Nautilus library ([link](https://users.cecs.anu.edu.au/~bdm/nauty/) or [link](https://pallini.di.uniroma1.it/))
\\ also geng, directg, showg utilities (maybe more)

* Ortools library ([install](https://developers.google.com/optimization/install?hl=ru) or use ``python -m pip install ortools --break-system-paskages``)

> Make sure both Nautilus and Ortools are located at default system paths (for example, /usr/local/ on Linux). You can manually move all packages with commands like ``sudo cp -rf /path/to/ortools/<xxx>/* /usr/local/<xxx>`` or specify path to a directory in CMakeLists.txt by adding ``set(CMAKE_PREFIX_PATH /path/to/libraries/)`` right before ``find_package(...)`` line.


## Building

* If you have troubles building, try removing garbage from previous builds
```
rm -rf build/
```

* Configuring **DEBUG** version (inlcude debug info; unoptimized, sanitized and more permissive)
```
cmake -B build/ -S . -DCMAKE_BUILD_TYPE=DEBUG
```

* Configuring **RELEASE** version (do not include debug info; optimized)
```
cmake -B build/ -S . -DCMAKE_BUILD_TYPE=RELEASE
```

* Building executable
```
cmake --build ./build/ --target nash-2.0 -j8
```

* Building sat tests
```
cmake --build ./build/ --target sat-test -j8
```


