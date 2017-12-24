mkdir -p build
cd build
mkdir -p imports
g++ -std=c++11 -I ../include -shared -fPIC -s -O3 ../darwin.cpp -o ./imports/darwin.cse