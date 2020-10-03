mkdir temp
g++ CRA_gen.cpp -o o_cra
g++ testbench_gen.cpp -o o_test

./o_cra 16 temp/CRA.v
./o_test 16 temp/testbench.v CRA

cd temp

iverilog testbench.v
./a.out > ../results

cd ..

rm o_cra o_test
rm -r temp
