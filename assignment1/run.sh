mkdir temp
g++ CRA_gen.cpp -o o_cra
g++ CLA_gen.cpp -o o_cla
g++ testbench_gen.cpp -o o_test

./o_cra 16 temp/CRA.v
./o_cla 16 temp/CLA.v
./o_test 16 temp/cra_testbench.v CRA
./o_test 16 temp/cla_testbench.v CLA

cd temp

iverilog cra_testbench.v
./a.out > ../cra_results

iverilog cla_testbench.v
./a.out > ../cla_results

cd ..

rm o_cra o_cla o_test
rm -r temp
