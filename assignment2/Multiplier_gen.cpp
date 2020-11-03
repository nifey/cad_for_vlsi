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

void print_csa(ofstream& outfile) {
	// This function prints the parameterized Carry Save adder into the file
	outfile << "module CSA(a, b, c, sum, carry);" << endl;
	outfile << "parameter ALEN = 8;" << endl;
	outfile << "parameter BLEN = 8;" << endl;
	outfile << "parameter CLEN = 8;" << endl;
	outfile << endl;
	outfile << "input [ALEN-1:0] a;" << endl;
	outfile << "input [BLEN-1:0] b;" << endl;
	outfile << "input [CLEN-1:0] c;" << endl;
	outfile << "output [ALEN-1:0] sum;" << endl;
	outfile << "output [BLEN-1:0] carry;" << endl;
	outfile << endl;
	outfile << "genvar i;" << endl;
	outfile << "generate" << endl;
	outfile << "\tfor (i=0; i<CLEN; i=i+1) begin" << endl;
	outfile << "\t\tassign #6 sum[i] = a[i] ^ b[i] ^ c[i];" << endl;
	outfile << "\tend" << endl;
	outfile << "\tfor (i=CLEN; i<BLEN; i=i+1) begin" << endl;
	outfile << "\t\tassign #3 sum[i] = a[i] ^ b[i];" << endl;
	outfile << "\tend" << endl;
	outfile << "\tfor (i=BLEN; i<ALEN; i=i+1) begin" << endl;
	outfile << "\t\tassign sum[i] = a[i];" << endl;
	outfile << "\tend" << endl;
	outfile << "\tfor (i=0; i<CLEN; i=i+1) begin" << endl;
	outfile << "\t\tassign #2 carry[i] = (a[i]&b[i])|(b[i]&c[i])|(c[i]&a[i]);" << endl;
	outfile << "\tend" << endl;
	outfile << "\tfor (i=CLEN; i<BLEN; i=i+1) begin" << endl;
	outfile << "\t\tassign #2 carry[i] = (a[i]&b[i]);" << endl;
	outfile << "\tend" << endl;
	outfile << "endgenerate" << endl;
	outfile << endl;
	outfile << "endmodule" << endl;
}

void print_cla(int n, ofstream& outfile) {
	// This function prints the CLA for n bits into the file
	
	// This is the number of reduction stages required
	int num_stages = log2(n) + 1;

	// Print the status module
	// The status module generates the status initially
	outfile << "module status(a, b, st);" << endl;
	outfile << "input a, b;" << endl;
	outfile << "output [1:0] st;" << endl;
	outfile << "assign #2 st[1] = a & b;" << endl;
	outfile << "assign #2 st[0] = a | b;" << endl;
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
	outfile << "module CLA (a, b, c);" << endl;
	outfile << "input [" << n-1 << ":0] a, b;" << endl;
	outfile << "output [" << n << ":0] c;" << endl;

	// We need (num_stages + 1) wire arrays to hold intermediate status values during reduction
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
	outfile << "assign c[" << n << "] = st" << num_stages << "[" << n-1 << "][0];" << endl;
	outfile << endl;

	outfile << "endmodule" << endl << endl;
}

void print_multiplier(int n, ofstream& outfile) {

	outfile << "module Multiplier (a, b, c);" << endl;
	outfile << "input [" << n-1 << ":0] a, b;" << endl;
	outfile << "output [" << 2*n-1 << ":0] c;" << endl;
	outfile << endl;

	// Calculate partial products
	outfile << "wire [" << n-1 << ":0] pp[" << n-1 << ":0];" << endl << endl;
	for(int i=0; i<n; i++) {
		for(int j=0; j<n; j++) {
			outfile << "assign #2 pp[" << i << "][" << j << "] = a[" << j << "] & b[" << i << "];" << endl;
		}
		outfile << endl;
	}
	
	outfile << "endmodule" << endl;
}

int main(int argc, char** argv) {
	if (argc < 4) {
		cout << "Usage: ./a.out n k output_filename" << endl;
		return 0;
	}
	int n = atoi(argv[1]);
	int k = atoi(argv[2]);
	string filename(argv[3]);
	cout << "Generating a Wallace Tree Multiplier for " << n << " bits" << endl;
	cout << "Output file: " << filename << endl;

	// Open the file to write output
	ofstream outfile;
	outfile.open(filename, ios::out | ios::trunc);
	if (!outfile.is_open()) {
		cout << "Error: Cannot open file " << filename << " for writing" << endl;
		return 0;
	}

	// Print the parameterized CSA
	print_csa(outfile);
	
	// Print the CLA required
	print_cla(2*n - 1, outfile);
	
	// Print the Multiplier module
	print_multiplier(n, outfile);

	// Close the output file
	outfile.close();
}
