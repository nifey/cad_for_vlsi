#define MAX_TESTCASES 10

#include<iostream>
#include<fstream>

using namespace std;

// This function generates a random hex of n*4 bits
string rand_hex(int n) {
	string hex;
	int val;
	while (n--) {
		val = rand() % 16;
		if (val < 10) {
			hex += ('0' + val);
		} else {
			hex += ('a' + (val - 10));
		}
	}
	return hex;
}

int main(int argc, char** argv) {
	if (argc < 5) {
		cout << "Usage: ./a.out n cycle_time num_stages output_filename" << endl;
		return 0;
	}
	int n = atoi(argv[1]);
	int cycle_time = atoi(argv[2]);
	if (cycle_time % 2 == 1) {
		cycle_time ++;
	}
	int num_stages = atoi(argv[3]);
	string filename(argv[4]);
	cout << "Generating testbench for Multiplier of " << n << " bits" << endl;
	cout << "Output file: " << filename << endl;

	// Open the file to write output
	ofstream outfile;
	outfile.open(filename, ios::out | ios::trunc);
	if (!outfile.is_open()) {
		cout << "Error: Cannot open file " << filename << " for writing" << endl;
		return 0;
	}

	// Include the module verilog file
	outfile << "`include \"Multiplier.v\"" << endl << endl;

	// Begin module
	outfile << "module testbench;" << endl;
	outfile << "reg [" << n-1 << ":0] a, b;" << endl;
	outfile << "wire [" << 2*n-1 << ":0] c;" << endl;
	outfile << "reg clk;" << endl;
	outfile << "Multiplier m(a, b, c, clk);" << endl << endl;

	outfile << "initial" << endl;
	outfile << "begin" << endl;
	outfile << "\tclk = 0;" << endl;
	outfile << "end" << endl;
	outfile << "always #" << (cycle_time/2) << " clk = ~clk;"  << endl;

	outfile << "initial" << endl;
	outfile << "begin" << endl;

	srand((unsigned) time(NULL));
	int count = 0;

	outfile << "\t#1 a=" << n << "'h0;b=" << n << "'h0;" << endl;
	// Generate MAX_TESTCASES number of testcases
	if (n < 4) {
		while (count < MAX_TESTCASES) {
			outfile << "\t#" << cycle_time << " a=" << n << "'h" << ('0' + rand()%(1<<n)) << ";b=" << n << "'h" << ('0' + rand()%(1<<n)) << ";" << endl;
			count++;
		}
	} else {
		while (count < MAX_TESTCASES) {
			outfile << "\t#" << cycle_time << " a=" << n << "'h" << rand_hex(n/4) << ";b=" << n << "'h" << rand_hex(n/4) << ";" << endl;
			count++;
		}
	}

	outfile << "\t#" << cycle_time * num_stages << " $finish;" << endl;
	outfile << "end" << endl << endl;

	// Create a monitor on the wires
	outfile << "always #" << (cycle_time/2) << " $display($time, \" %h x %h = %h\", a, b, c);" << endl;

	// End module
	outfile << "endmodule" << endl;

	// Close the output file
	outfile.close();
}
