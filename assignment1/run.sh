if [ $# -ne 1 ];
then
	echo "Usage: bash run.sh N"
	exit
fi

if [ -d output ];
then
	rm -r output
fi

mkdir output
g++ CRA_gen.cpp -o o_cra
g++ CLA_gen.cpp -o o_cla
g++ testbench_gen.cpp -o o_test

./o_cra $1 output/CRA.v
./o_cla $1 output/CLA.v
./o_test $1 output/cra_testbench.v CRA
./o_test $1 output/cla_testbench.v CLA
rm o_cra o_cla o_test

cd output

echo 'Running CRA testbench'
iverilog cra_testbench.v
./a.out > cra_results

echo 'Running CLA testbench'
iverilog cla_testbench.v
./a.out > cla_results

rm a.out

echo 'Analysing CRA results'
bash ../analyse.sh cra_results
echo 'Analysing CLA results'
bash ../analyse.sh cla_results

cd ..
