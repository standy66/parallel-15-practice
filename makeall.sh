#! /bin/bash
rm -rf debug
rm -rf release
mkdir debug
mkdir release

cd debug
cmake -DCMAKE_BUILD_TYPE=Debug .. && make
cd ..

cd release
cmake -DCMAKE_BUILD_TYPE=Release .. && make
cd ..
