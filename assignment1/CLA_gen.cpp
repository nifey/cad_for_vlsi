#include<iostream>
#include<fstream>
using namespace std;

// Function to find the log to base 2
int log2(int n) {
	if (n==1) {
		return 0;
	} else {
		return log2(n/2) + 1;
	}
}

int main(int argc, char** argv) {
	if (argc < 3) {
		cout << "Usage: ./a.out n output_filename" << endl;
		return 0;
	}
	int n = atoi(argv[1]);
	string filename(argv[2]);
	cout << "Generating a Carry lookahead adder for " << n << " bits" << endl;
	cout << "Output file: " << filename << endl;

	// Open the file to write output
	ofstream outfile;
	outfile.open(filename, ios::out | ios::trunc);
	if (!outfile.is_open()) {
		cout << "Error: Cannot open file " << filename << " for writing" << endl;
		return 0;
	}

	// This is the number of reduction stages required
	int num_stages = log2(n) + 1;

	// Print the status module
	// The status module generates the status initially
	outfile << "module status(a, b, st);" << endl;
	outfile << "input a, b;" << endl;
	outfile << "output [1:0] st;" << endl;
	outfile << "assign #1 st[1] = a & b;" << endl;
	outfile << "assign #1 st[0] = a | b;" << endl;
	outfile << "endmodule" << endl;

	outfile << endl;

	// Print the reduce module
	// The reduce module is the star operator
	outfile << "module reduce(sta, stb, stc);" << endl;
	outfile << "input [1:0] sta, stb;" << endl;
	outfile << "output [1:0] stc;" << endl;
	outfile << "assign #2 stc[0] = stb[1] | (sta[0] & stb[0]) ;" << endl;
	outfile << "assign #2 stc[1] = stb[1] | (sta[1] & stb[0]) ;" << endl;
	outfile << "endmodule" << endl;

	outfile << endl;

	// Now print the CLA module
	outfile << "module CLA (a, b, c, cout);" << endl;
	outfile << "input [" << n-1 << ":0] a, b;" << endl;
	outfile << "output [" << n-1 << ":0] c;" << endl;
	outfile << "output cout;" << endl;

	// We need n-1 carry wires to interconnect the adders
	outfile << "wire [1:0] ";
	for (int i=0; i <= num_stages ; i++) {
		outfile << "st" << i << "[" << n-1 << ":0]";
		if (i != num_stages) {
			outfile << ", ";
		}
	}
	outfile << ";" << endl;
	outfile << endl;

	// Phase 1: Calculate initial status bits
	for (int i=0; i<n; i++) {
		outfile << "status s" << i << "(a[" << i << "], b[" << i << "], st0[" << i << "]);" << endl;
	}
	outfile << endl;

	// Phase 2: Reduction stages
	int diff = 1;
	for (int i=1; i <= num_stages; i++) {
		// Values that are already reduced
		int j=0;
		for (; j < diff-1; j++) {
			outfile << "assign st" << i << "[" << j <<"] = st" << i-1 << "[" << j <<"];" << endl;
		}
		// Special case: when prev status is Kill (by default)
		outfile << "reduce R"<< i-1 << "_" << j <<"(2'h0, st" << i-1 << "[" << j << "], st" << i << "[" << j << "]);" << endl;
		// Normal case reduction
		for (j++; j < n; j++) {
			outfile << "reduce R"<< i-1 << "_" << j <<"(st" << i-1 << "[" << j-diff << "], st" << i-1 << "[" << j << "], st" << i << "[" << j << "]);" << endl;
		}
		outfile << endl;
		diff *= 2;
	}

	// Phase 3: Perform final addition
	outfile << "assign #6 c[0] = a[0] ^ b[0] ^ 0;" << endl;
	for (int i=1; i<n; i++) {
		outfile << "assign #6 c[" << i << "] = a[" << i << "] ^ b[" << i << "] ^ st"<< num_stages << "[" << i-1 << "][0];" << endl;
	}
	outfile << "assign cout = st" << num_stages << "[" << n-1 << "][0];" << endl;
	outfile << endl;

	outfile << "endmodule" << endl;

	// Close the output file
	outfile.close();
}
