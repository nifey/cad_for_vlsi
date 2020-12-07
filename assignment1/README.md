# Assignment 1

Details of the files:
1. CRA_gen.cpp : C++ code that generates Carry Ripple adder
2. CLA_gen.cpp : C++ code that generates Carry Lookahead adder
3. testbench_gen.cpp : C++ code that generates testbench
4. run.sh : Bash script to generate the CLA, CRA, testbench and run them
5. analyse.sh : Bash script to check if the adder outputs are correct and also find the average latency of the adders

## Instruction for running the code

I have written run.sh to make it easier to run the generator codes
You can simply run 
$ bash run.sh N
where N is the size of the adder

Eg: To test for a 64 bit adder type
$ bash run.sh 64

This will do the following:
- compile CRA_gen.cpp CLA_gen.cpp testbench_gen.cpp
- generate verilog files: CRA.v CLA.v cra_testbench.v cla_testbench.v
- run the verilog testbenches and generate results in cra_results and cla_results
- check for errors in the output and find the min, max and average latency for both CRA and CLA

The generated verilog files will be in the output directory. There will be 6 files in the output directory:
1. CRA.v : Verilog file with Carry Ripple Adder module
2. CLA.v : Verilog file with Carry Lookahead Adder module
3. cra_testbench.v : Testbench for CRA with 100000 random testcases
4. cla_testbench.v : Testbench for CLA with 100000 random testcases
5. cra_results : Output of CRA execution
6. cla_results : Output of CRA execution

You can get the min, max, average latency by using the analyse.sh script as follows:
$ bash analyse.sh output/cra_results

## Comparision of Carry Ripple Adder and Carry Lookahed Adder

To compare Carry Ripple Adder and Carry Lookahead Adder, I calculated the latency it took for the result to stabilize for 100000 randomly generated test cases using both adders.
The average latency of both Carry Ripple Adder and Carry Lookahead adder for different values of N is shown below.

|N	|Carry Ripple |	Carry Lookahead |
|:---|:-----------|:----------------|
|2	|8.491273	|8.491273|
|4	|10.208891	|10.552686|
|8	|14.464361	|13.557030|
|16	|18.836687	|15.953534|
|32	|22.976565	|17.999091|
|64	|27.048404	|20.000000|
|128	|31.052667	|22.000000|
|256	|35.095010	|24.000000|
|512	|39.147677	|26.000000|

Observations:
1. The average latency of Carry Ripple adder is equal to or better than that of Carry Lookahead Adder for N=2,4 but for higher values of N, Carry Lookahead Adder consistently performs better.

2. The latency of Carry Lookahead adder becomes constant for higher values of N i.e. every addition with Carry Lookahead adder takes constant time independant of the input values. Starting from 64 bit adder the latency of Carry Lookahead adder increases in steps of 2 units (because of the extra Reduce level introduced).

3. The latency of Carry Ripple Adder depends on the input values and so it takes less time for some inputs and takes a long time for some inputs.

4. Eventhough the average latency of Carry Ripple adder is quite comparable to that of Carry Lookahead adder, there are some case that takes a longer latency which might not be accounted in this data because we use 100000 randomly generated testcases for addition.

5. For an n bit Carry Ripple Adder, the maximum latency is calculated as (n-1)(Time to generate carry) + (Time to generate sum). Substituing the delay values we get the maximum latency as 4(n-1) + 6 units.

The Carry Ripple Adder has the maximum latency when the carry has to propagate all the way from the LSB to MSB. For example, adding 0x00000001 and 0xffffffff using a 8 bit Carry Ripple Adder takes 34 units which is the maximum latency of a 8 bit Carry Ripple Adder, also calculated by the above formula (4(7) + 6 = 34 units)

This makes a huge difference when N is large. For instance, when N=512, for adding 0x0000.....0001 (512 bits) and 0xffff....ffff (512 bits), Carry Ripple adder takes 2050 units while Carry Lookahead adder takes 26 units.

6. In conclusion, the key advantages of Carry Lookahead adder over Carry Ripple adder is that it has a lower latency and that latency is constant regardless of the inputs.
