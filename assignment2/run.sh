if [ $# -ne 2 ];
then
	echo "Usage: bash run.sh N K"
	exit
fi

if [ -d output ];
then
	rm -r output
fi

mkdir output

# Compiling generators
g++ Multiplier_gen.cpp -o o_mult
g++ testbench_gen.cpp -o o_test

# Generating verilog code
./o_mult $1 $2 output/Multiplier.v
./o_test $1 output/testbench.v
rm o_mult o_test

cd output

# Running multiplier
echo 'Running testbench'
iverilog testbench.v
./a.out > results
rm a.out

# Analysing adder outputs
echo '>> Analysing results'
bash ../analyse.sh results

echo 'The generated verilog code is present in output directory'

cd ..
