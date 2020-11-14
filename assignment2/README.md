# Assignment 2

Details of the files:
1. Multiplier_gen.cpp : C++ code that generates Pipelined Wallace tree multiplier
2. testbench_gen.cpp : C++ code that generates testbench
3. run.sh : Bash script to generate the CLA, CRA, testbench and run them
4. analyse.sh : Bash script to check if the multiplier outputs are correct

## Instruction for running the code

I have written run.sh to make it easier to run the generator codes
You can simply run 
$ bash run.sh N K
where N is the size of the adder and K is the number of stages after which there should be a pipeline register

Eg: To generate a 64 bit multiplier that has a pipeline register after every 3 stages of Wallace tree multiplier type
$ bash run.sh 64 3

This will do the following:
- compile Muliplier_gen.cpp testbench_gen.cpp
- generate verilog files: Multiplier.v testbench.v
- run the verilog testbenches and generate results in results
- check for errors in the output

The generated verilog files will be in the output directory. There will be 3 files in the output directory:
1. Multiplier.v : Verilog file with Wallace tree multiplier module
2. testbench.v : Testbench for Multiplier with 10 random testcases
3. results : Output of multiplier execution

You can check for correctness of the output by typing
$ bash analyse.sh output/results <num_stages>
num_stages is the number of pipeline stages in the multiplier. This value is printed by the Multiplier_gen.cpp program.

## Analysis

I have considered N = 128 bits for this study. The data for 128 bit Wallace tree multiplier with varying K values is shown in the table below.

- Num stages			: Total number of pipeline stages in the multiplier
- Pipeline stage time		: The delay of the pipeline stage with max delay. This will determine the min clock cycle time at which the multiplier runs without error.
- Number of bits in registers	: The total number of bits stored in Registers. This is the area overhead of the multiplier.

-------------------------------------------------
| K	|  Num	|    Pipeline	| Number of bits|
|	|Stages	|   stage time	|  in registers	|
|	|	|		|    (Area)	|
-------------------------------------------------
|1	|12	|24		|37522		|
|2	|7	|24		|16133		|
|3	|5	|24		|9135		|
|4	|4	|26		|5837		|
|5	|4	|32		|4415		|
|6	|3	|38		|2933		|
|7	|3	|44		|2423		|
|8	|3	|50		|2182		|
|9	|3	|56		|1945		|
|10	|3	|62		|1759		|
|11	|2	|68		|1026		|
|12	|2	|68		|1026		|
|13	|2	|68		|1026		|
|14	|2	|68		|1026		|
|15	|2	|68		|1026		|
-------------------------------------------------

Observations:
1. For small values of K (upto 3), the delay of the 256 bit Carry Lookahead Adder (24 units) becomes the pipeline stage with the maximum delay. But starting from K = 4, the delay of pipeline stage consisting of Carry Save Adders starts to dominate.

2. The max delay of pipeline stage increases as we increase K, because for increasing values of K, the number of Carry Save Adder stages that are put in the same pipeline stage is increased and so we have to wait for a longer time so that data is propagated to all the Carry Save Adders in the pipeline stage and the final CSA stage of the pipeline stage produces its output.

3. Each Carry Save Adder has a delay of 6 units and so we can observe that the latency increases in steps of 6 units (for K >= 4) because of addition of CSA stages.

4. For all values of K greater than or equal to 11, all Carry Save Adder stages of the multiplier become part of the same pipeline stage. And so the latency is constant (68 units for K>=11) after that. When K >= 11, we only have two pipeline stages. One stage is for computing the partial products and reducing them to two values using Carry Save Adders. The second stage is for computing the final product using the Carry Lookahead Adder.

5. The Area overhead is very high (37522) for K = 1 where a pipeline register is added after every Carry Save Adder stage of the multiplier. The Area overhead decreases as we increase K. This is obvious because as we increase K, the number of pipeline stages is decreased and so the number of registers needed to store intermediate values between pipeline stages also reduces. The minimum area overhead (1026) is for K>=11 where there are only two pipeline stages.

6. The Latency and throughput data of the 128 bit multiplier is given in the table below. We can observe that the number of operations completed in 1000 units decreases as we increase K. Ideally the multiplier should complete one multiplication operations every cycle. But increasing K increases the cycle time which reduces the throughput.

7. From the table below, we can observe that the multiplication latency doesn't seem to follow strict pattern but is generally worse when K is too low or too high. Based on the data, K = 4 is the ideal choice for the 128 bit multiplier because it has a high throughput (38 operations per 1000 time units) and low latency (104 time units).


- Multiplication Latency	: Time between providing input and obtaining the product on the output lines. This can be calculated as (Pipeline stage time * number of pipeline stages)
- Operations per 1000 units	: The number of multiplication operations that are completed in 1000 units of time. This is calculated as the (1000/Pipeline stage time). This is a measure of throughput of the multiplier.
-------------------------------------------------
| K	|Multiplication	| Operations completed	|
|	|   Latency	|   per 1000 units	|
|	|   (Latency)	|    (Throughput)	|
-------------------------------------------------
|1      |288            |41                     |
|2      |168            |41                     |
|3      |120            |41                     |
|4      |104            |38                     |
|5      |128            |31                     |
|6      |114            |26                     |
|7      |132            |22                     |
|8      |150            |20                     |
|9      |168            |17                     |
|10     |186            |16                     |
|11     |136            |14                     |
|12     |136            |14                     |
|13     |136            |14                     |
|14     |136            |14                     |
|15     |136            |14                     |
-------------------------------------------------
