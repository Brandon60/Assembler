// Brandon Gonzales, 727002611
// project 6

#include <iostream>
#include <fstream>
#include <map>
#include <iterator>
#include <string>
#include <bitset>
using namespace std;

// Function getNumArgs obtains the amount of token arguments corresponding to the category/ type of 
// instruction.
int getNumArgs(string line, string opName){
	// map contains data of type of instruction and its corresponding number of token arguments
	map<string, int> ops = { {"ADD", 3}, {"ADDI", 2}, {"SUB", 3}, {"SUBI", 2},{"NAND", 3}, {"NOR", 3}, 
		    {"READ", 2}, {"WRITE", 2}, {"JMP", 1},
			{"BEQ", 2}, {"INP", 1}, {"OUT", 2}};
	try{ 		// using try & catch to check if opName is in the map to ensure we get no unexpected errors
		ops.at(opName);
	} catch(const exception& e){
		return -1;
	}
	return ops.at(opName);
}

// We use checkValidity to determine if characters between tokens / token arguments
// are either commas or spaces. We also designate if we're checking between tokens
// or token arguments with the parameter type. 
bool checkValidity(string line, int pos1, int pos2, int type){
	// Data used to determine if valid.
	int space = 0;
	int comma = 0;
	// loop checks through substring 
	for(int i = pos1; i < pos2; i++){
		if(line[i] == ',' || isspace(line[i])){
			if(isspace(line[i])){
				space = 1;
			}
			if(line[i] == ','){
				comma = 1;
			}
			continue;
		}
		// if a character is neither comma or space, return false.
		else{
			return 0;
		}
	}
	// if we're checking between tokens (i.e. BEQ R0)
	if(type == 0){
		if(space){
			return 1;
		}
	}
	// if we're checking between token arguments (i.e. R0, R1)
	if(type > 0){
		if(space && comma){
			return 1;
		}
	}
	// if requirements not met return false (i.e. JMP,R0)
	return 0;
}

// Similar to getNumArgs, but returns corresponding OPCODE given the name of the type of instruction.
string opCode(string line, string opName){
	map<string, string> ops = { {"ADD", "0000"}, {"ADDI", "0001"}, {"SUB", "0010"}, {"SUBI", "0011"}, {"NAND", "0100"}, {"NOR", "0110"}, 
		    {"READ", "1000"}, {"WRITE", "1010"}, {"JMP", "1011"},
			{"BEQ", "1001"}, {"INP", "1110"}, {"OUT", "1100"}};
	try{
		ops.at(opName);
	} catch(const exception& e){
		return "error"; // we return error here to actually determine if opName is valid.
	}
	return ops.at(opName);
}

// We use intToBinary to convert decimal to binary using bitset.
// We use the parameter type to designate if we are translating register numbers or read in numbers. 
string intToBinary(int a, int type){
	if(type == 0){
		string out = bitset<3>(a).to_string();
		return out;
	}
	else if(type == 1){
		string out = bitset<6>(a).to_string();
		return out;
	}
	else{
		// no purpose for this else statement, used as a "just in case"
		return "error";
	}
}

// Primary function to determine translation, calls functions above.
string parser(string line){
	// Data output used to store translation.
	string output = "";
	// if line is a comment
	if(line.substr(0, 2) == "//"){
		return "";
	}
	// if line is empty
	else if(line.empty()){
		return "";
	}
	// translation
	else{
		// obtain name of operation
		string opName = "";
		for(char x : line){
			if(x == ',' || x == ' '){
				break;
			}
			else{
				opName += x;
			}
		}
		// check if opName is valid
		if(opCode(line, opName) == "error" || !(checkValidity(line, opName.length(), line.find("R", opName.length()), 0))){
			return "error";
		} 
		// add it to the output
		output += opCode(line, opName);
		int pos = opName.length();
		int newPos, num;
		// This loop checks if spaces between token / token arguments are valid, and if they are 
		// it converts token argument decimal values to binary with intToBinary function
		// and adds it to the output.
		for(int i = 0; i < getNumArgs(line, opName); i++){
			newPos = line.find("R", pos);
			if(!(checkValidity(line, pos, newPos, i))){
				return "error";
			}
			num = line[newPos+1] - 48;
			pos = newPos+2;
			output += intToBinary(num, 0);
		}
		// In the case we are dealing with number being read in,
		// this if statement handles obtaining the read in number.
		if(opName == "ADDI" || opName == "SUBI"){
			string dig;
			// hitDig is used to determine when the entire number has been obtained
			bool hitDig = false;
			for(int i = pos; i < line.length(); i++){
				if(isdigit(line[i])){
					dig += line[i];
					hitDig = true;
					pos = i+1;
				}
				else if(hitDig){
					break;
				}
			}
			// finally, add obtained decimal number to output as binary conversion
			output += intToBinary(stoi(dig), 1);
		}
		// This if statement handles everything after the instruction, including
		// comments, or invalid characters.
		if(pos < line.length()){
			for(int i = pos; i < line.length(); i++){
				if(i+2 < line.length()){
					// if we observe a comment, ignore the rest of the line
					if(line.substr(i, 2) == "//"){ 
						break;
					}
				}
				if(!(isspace(line[i]))){
					return "error";
				}
			}
		}
		// Most of the time, the instruction has "DON'T CARE" values, so we append varying amounts
		// of '0's to the string until the valid number of bits is met.
		while(output.length() < 16){
			output += "0";
		}

	}
	return output;
}
// main is used to read/ write in the file
int main(){
	string fileName;
	string line;
	cout << "Enter file: " << endl;
	cin >> fileName;
	ifstream infile(fileName);
	ofstream myfile;
	myfile.open(fileName.substr(0, fileName.length()-4) += ".hack");
	// loop used to pass each line to parser
	while(getline(infile, line)){
		string input = parser(line);
		// if statement takes care of outputing the proper error message
		if(input == "error"){
			cout << "SYNTAX ERROR: Illegal Instruction Encountered." << endl;
			break;
		}
		else{
			// this if is used to cover for comment lines or empty lines
			if(input == ""){
				continue;
			}
			// writing into the file
			else{
				myfile << input << endl;
			}
		}
	}
	myfile.close();
	return 0;
}