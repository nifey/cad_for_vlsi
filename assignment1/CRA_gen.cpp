#include<iostream>
#include<fstream>
using namespace std;

int main(int argc, char** argv) {
	if (argc < 3) {
		cout << "Usage: ./a.out n output_filename" << endl;
		return 0;
	}
	int n = atoi(argv[1]);
	string filename(argv[2]);
	cout << "Generating a Carry ripple adder for " << n << " bits" << endl;
	cout << "Output file: " << filename << endl;

	// Open the file to write output
	ofstream outfile;
	outfile.open(filename, ios::out | ios::trunc);
	if (!outfile.is_open()) {
		cout << "Error: Cannot open file " << filename << " for writing" << endl;
		return 0;
	}

	// Print the adder module first
	outfile << "module adder(a,b,cin,s,cout);" << endl;
	outfile << "input a,b,cin;" << endl;
	outfile << "output s, cout;" << endl;
	outfile << "assign #6 s = a^b^cin;" << endl;
	outfile << "assign #4 cout = (a&b) | (b&cin) | (cin&a);" << endl;
	outfile << "endmodule" << endl;

	outfile << endl;

	// Now print the CRA module
	outfile << "module CRA (a, b, c, cout);" << endl;
	outfile << "input [" << n-1 << ":0] a, b;" << endl;
	outfile << "output [" << n-1 << ":0] c;" << endl;
	outfile << "output cout;" << endl;

	// We need n-1 carry wires to interconnect the adders
	outfile << "wire ";
	for (int i=0; i < n-1 ; i++) {
		outfile << "carry" << i;
		if (i != n-2) {
			outfile << ", ";
		}
	}
	outfile << ";" << endl;

	// First adder (takes 0 as cin)
	outfile << "adder A0(a[0],b[0],1'b0,c[0],carry0);" << endl;

	// Intermediate adders
	for (int i=1; i < n-1; i++) {
		outfile << "adder A" << i << "(a[" << i << "],b[" << i << "],carry" << i-1 << ",c[" << i << "],carry" << i << ");" << endl;
	}

	// Last adder (outputs the cout)
	outfile << "adder A" << n-1 << "(a[" << n-1 << "],b[" << n-1 << "],carry" << n-2 << ",c[" << n-1 << "],cout);" << endl;

	outfile << "endmodule" << endl;

	// Close the output file
	outfile.close();
}
