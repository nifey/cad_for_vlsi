#define INTERVAL 10000
#define MAX_TESTCASES 100000

#include<iostream>
#include<fstream>
#include<vector>
using namespace std;

void next_hex(string *hex) {
	int n = hex->size();
	int i = n-1;
	bool carry = false;
	do {
		carry = false;
		switch ((*hex)[i]){
			case '0':
				(*hex)[i] = '1';
				break;
			case '1':
				(*hex)[i] = '2';
				break;
			case '2':
				(*hex)[i] = '3';
				break;
			case '3':
				(*hex)[i] = '4';
				break;
			case '4':
				(*hex)[i] = '5';
				break;
			case '5':
				(*hex)[i] = '6';
				break;
			case '6':
				(*hex)[i] = '7';
				break;
			case '7':
				(*hex)[i] = '8';
				break;
			case '8':
				(*hex)[i] = '9';
				break;
			case '9':
				(*hex)[i] = 'a';
				break;
			case 'a':
				(*hex)[i] = 'b';
				break;
			case 'b':
				(*hex)[i] = 'c';
				break;
			case 'c':
				(*hex)[i] = 'd';
				break;
			case 'd':
				(*hex)[i] = 'e';
				break;
			case 'e':
				(*hex)[i] = 'f';
				break;
			case 'f':
				(*hex)[i] = '0';
				carry = true;
				break;
			default:
				break;
		}
		i--;
	} while (carry);
}

vector<string> generate_hex_numbers(int n) {
	vector<string> hex_list;
	string start;
	int total_number = 1;
	for(int i=0; i<n; i++){
		total_number *= 2;
	}
	for(int i=0; i<(((n-1)/4)+1); i++) {
		start += '0';
	}
	hex_list.push_back(start);
	for(int i=0; i<total_number - 1; i++) {
		next_hex(&start);
		hex_list.push_back(start);
	}
	return hex_list;
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

	// Generate all possible hex values to be used as input
	vector<string> numbers = generate_hex_numbers(n);

	// Include the module verilog file
	// (Assuming the file is named module.v)
	outfile << "`include \"" << module << ".v\"" << endl << endl;

	// Begin module
	outfile << "module testbench;" << endl;
	outfile << "reg [" << n-1 << ":0] a, b;" << endl;
	outfile << "wire [" << n-1 << ":0] c;" << endl;
	outfile << "wire cout;" << endl << endl;
	outfile << "CRA A(a, b, c, cout);" << endl << endl;

	outfile << "initial" << endl;
	outfile << "begin" << endl;
	int count = 0;
	for(int i=0; i<numbers.size(); i++) {
		for(int j=0; j<numbers.size(); j++) {
			outfile << "\t#" << INTERVAL << " a=" << n << "'h" << numbers[i] << ";b=" << n << "'h" << numbers[j] << ";" << endl;
			count++;
			if (count == MAX_TESTCASES) {
				break;
			}
		}
		if (count == MAX_TESTCASES) {
			break;
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
