#!/bin/bash

cmake -B build/ -S . --DCMAKE_BUILD_TYPE=RELEASE
cmake --build build/ --target nash-2.0
./build/nash-2.0
