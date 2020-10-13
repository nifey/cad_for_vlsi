#define INTERVAL 10000000
#define MAX_TESTCASES 100000

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
	if (argc < 4) {
		cout << "Usage: ./a.out n output_filename CRA/CLA" << endl;
		return 0;
	}
	int n = atoi(argv[1]);
	string filename(argv[2]), module(argv[3]);
	cout << "Generating testbench for module " << module << " of " << n << " bits" << endl;
	cout << "Output file: " << filename << endl;

	// Open the file to write output
	ofstream outfile;
	outfile.open(filename, ios::out | ios::trunc);
	if (!outfile.is_open()) {
		cout << "Error: Cannot open file " << filename << " for writing" << endl;
		return 0;
	}

	// Include the module verilog file
	// (Assuming the file is named module.v)
	outfile << "`include \"" << module << ".v\"" << endl << endl;

	// Begin module
	outfile << "module testbench;" << endl;
	outfile << "reg [" << n-1 << ":0] a, b;" << endl;
	outfile << "wire [" << n-1 << ":0] c;" << endl;
	outfile << "wire cout;" << endl << endl;
	outfile << module << " A(a, b, c, cout);" << endl << endl;

	outfile << "initial" << endl;
	outfile << "begin" << endl;

	srand((unsigned) time(NULL));
	int count = 0;

	// Generate MAX_TESTCASES number of testcases
	if (n < 4) {
		while (count < MAX_TESTCASES) {
			outfile << "\t#" << INTERVAL << " a=" << n << "'h" << ('0' + rand()%(1<<n)) << ";b=" << n << "'h" << ('0' + rand()%(1<<n)) << ";" << endl;
			count++;
		}
	} else {
		while (count < MAX_TESTCASES) {
			outfile << "\t#" << INTERVAL << " a=" << n << "'h" << rand_hex(n/4) << ";b=" << n << "'h" << rand_hex(n/4) << ";" << endl;
			count++;
		}
	}

	outfile << "end" << endl << endl;

	// Create a monitor on the wires
	outfile << "initial" << endl;
	outfile << "begin" << endl;
	outfile << "\t$monitor($time, \" %h + %h = %b%h\", a, b, cout, c);" << endl;
	outfile << "end" << endl << endl;

	// End module
	outfile << "endmodule" << endl;

	// Close the output file
	outfile.close();
}
