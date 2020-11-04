#include<iostream>
#include<fstream>
#include<list>
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
	outfile << "endmodule" << endl;
	outfile << endl;
}

void print_cla(int n, ofstream& outfile) {
	// This function prints the CLA for n bits into the file
	
	// This is the number of reduction stages required
	int num_stages = log2(n);

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
	outfile << "output [" << n-1 << ":0] c;" << endl;

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
	outfile << endl;

	outfile << "endmodule" << endl << endl;
}

// This class is used to track the length of bits of each partial value
class PartialProduct {
	public:
		int level;	// Level at which it is used
		int id;		// Used for naming
		int length;	// Length of the partial product
		int shift;	// Number of bits after the number that are zero

		PartialProduct() {}

		PartialProduct(int _level, int _id, int _length, int _shift) {
			level = _level;
			id = _id;
			length = _length;
			shift = _shift;
		}

		void operator = (const PartialProduct &other) {
			level = other.level;
			id = other.id;
			length = other.length;
			shift = other.shift;
		}
};

void print_multiplier(int n, ofstream& outfile) {
	// This function prints the wallace tree multiplier

	int id;
	list<PartialProduct> values;

	outfile << "module Multiplier (a, b, c);" << endl;
	outfile << "input [" << n-1 << ":0] a, b;" << endl;
	outfile << "output [" << 2*n-1 << ":0] c;" << endl;
	outfile << endl;

	// Calculate partial products
	id = 0;
	outfile << "wire [" << n-1 << ":0] ";
	for(int i=0; i<n; i++) {
		outfile << "l0_"<< id;
		if (i != n-1) {
			outfile << ", ";
		} else {
			outfile << ";";
		}
		values.push_back(PartialProduct(0, id, n, id));
		id++;
	}
	outfile << endl << endl;
	for(int i=0; i<n; i++) {
		for(int j=0; j<n; j++) {
			outfile << "assign #2 l0_"<<  i << "[" << j << "] = a[" << j << "] & b[" << i << "];" << endl;
		}
		outfile << endl;
	}

	// Add the intermediate CSAs for all level
	int level = 1;
	while (values.size() > 2) {
		id = 0;
		int num_csa = values.size()/3;
		int remaining = values.size() % 3;

		while (num_csa--) {
			// Take three values of previous level and combine using a CSA
			PartialProduct a, b, c;
			c = values.front();
			values.pop_front();
			b = values.front();
			values.pop_front();
			a = values.front();
			values.pop_front();

			if (a.level != b.level || b.level != c.level) {
				cout << "Error: Levels are different" << endl;
			}

			int alen, blen, clen, min_shift;
			min_shift = a.shift;
			if (b.shift < c.shift && b.shift < min_shift) {
				min_shift = b.shift;
			} else if (c.shift < b.shift && c.shift < min_shift) {
				min_shift = c.shift;
			}

			alen = a.length + (a.shift - min_shift);
			blen = b.length + (b.shift - min_shift);
			clen = c.length + (c.shift - min_shift);

			if (!(alen >= blen && blen >= clen)) {
				cout << "Error: Length not in sorted order" << endl;
			}

			outfile << "wire ["<< alen-1 <<":0] l" << level << "_" << id << ";" << endl;
			outfile << "wire ["<< blen-1 <<":0] l" << level << "_" << id+1 << ";" << endl;
			outfile << "CSA #(" << alen << ", " << blen << ", " << clen << ") csa"<< level << "_" <<  num_csa << " (";

			if (a.shift > min_shift) {
				outfile << "{l" << a.level << "_" << a.id << ", "<< a.shift - min_shift <<"'b0}, ";
			} else {
				outfile << "l" << a.level << "_" << a.id << ", ";
			}

			if (b.shift > min_shift) {
				outfile << "{l" << b.level << "_" << b.id << ", "<< b.shift - min_shift <<"'b0}, ";
			} else {
				outfile << "l" << b.level << "_" << b.id << ", ";
			}

			if (c.shift > min_shift) {
				outfile << "{l" << c.level << "_" << c.id << ", "<< c.shift - min_shift <<"'b0}, ";
			} else {
				outfile << "l" << c.level << "_" << c.id << ", ";
			}

			outfile << "l" << level << "_" << id << ", ";
			outfile << "l" << level << "_" << id + 1;
			outfile << ");" << endl << endl;

			if (alen >= blen + 1) {
				values.push_back(PartialProduct(level, id+1, blen, min_shift+1));
				values.push_back(PartialProduct(level, id, alen, min_shift));
			} else {
				values.push_back(PartialProduct(level, id, alen, min_shift));
				values.push_back(PartialProduct(level, id+1, blen, min_shift+1));
			}
			id = id + 2;
		}

		while (remaining--) {
			// The remaining values have to be send to next level as it is
			PartialProduct a = values.front();
			values.pop_front();

			outfile << "wire ["<< a.length-1 <<":0] l" << level << "_" << id << ";" << endl;
			outfile << "assign l" << level << "_" << id << " = l" << a.level << "_" << a.id << ";" << endl << endl;
			a.level = level;
			a.id = id;

			id += 1;
			values.push_back(a);
		}

		level++;
	}

	// Add the CLA for the final stage
	PartialProduct a, b;
	a = values.front();
	values.pop_front();
	b = values.front();
	values.pop_front();
	if (values.size() != 0) {
		cout << "Error: List not empty" << endl;
	}
	outfile << "CLA cla(";

	// Adjust length to CLA size
	bool bracket_open = (a.length + a.shift < 2*n) || (a.shift != 0);
	if (bracket_open) {
		outfile << "{";
	}
	if (a.length + a.shift < 2*n) {
		outfile << (2*n) - (a.length + a.shift) << "'b0, " ;
	}
	outfile << "l" << a.level << "_" << a.id;
	if (a.shift != 0) {
		outfile << ", " << a.shift << "'b0";
	}
	if (bracket_open) {
		outfile << "}, ";
	} else {
		outfile << ", ";
	}

	bracket_open = (b.length + b.shift < 2*n) || (b.shift != 0);
	if (bracket_open) {
		outfile << "{";
	}
	if (b.length + b.shift < 2*n) {
		outfile << (2*n) - (b.length + b.shift) << "'b0, " ;
	}
	outfile << "l" << b.level << "_" << b.id;
	if (b.shift != 0) {
		outfile << ", " << b.shift << "'b0";
	}
	if (bracket_open) {
		outfile << "}, ";
	} else {
		outfile << ", ";
	}

	outfile << "c);" << endl;
	
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
	print_cla(2*n, outfile);
	
	// Print the Multiplier module
	print_multiplier(n, outfile);

	// Close the output file
	outfile.close();
}
