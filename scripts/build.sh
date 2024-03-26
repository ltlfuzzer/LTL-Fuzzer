mkdir build
cd build
cmake ../
make
cd ../AFLGo
make 
cd ./llvm_mode
make
cd ../distance_calculator
cmake -G Ninja ./
cmake --build ./
