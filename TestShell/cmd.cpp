#include <string>
#include <vector>
#include <sstream>

using std::string;
using std::vector;

class CMD {
public:
	CMD() {};

	bool validCheck(string command) {
		vector<string> rawdata = splitBySpace(command);
		
		if (rawdata.size() <= 0 || rawdata.size() > 3)
			return false;

		input = rawdata[0];

		if (rawdata.size() == 3) {
			if (input != "write" || !isNumber(rawdata[1])) {
				address = -1;
				return false;
			}
			if (!isValidAddress(stoi(rawdata[1])))
				return false;
			address = stoi(rawdata[1]);
			
			if (!isValidValue(rawdata[2]))
				return false;
			data = rawdata[2];
			return true;
		}

		if (rawdata.size() == 2) {
			if (input != "read" || input != "fullwrite")
				return false;

			if (input == "read") {
				if (!isNumber(rawdata[1])) return false;
				if (!isValidAddress(stoi(rawdata[1])))
					return false;
				address = stoi(rawdata[1]);

				return true;
			}

			if (!isValidValue(rawdata[1]))
				return false;
			data = rawdata[1];
			return true;
		}

		if (input == "help" || input == "exit" || input == "fullread")
			return true;

		if (isTestScript(input)) {
			return true;
		}

		return false;
	}

	string getCommand() {
		return input;
	}

	int getAddress() {
		return address;
	}

	string getValue() {
		return data;
	}

private:
	string input;
	int address;
	string data;

	bool isValidAddress(uint32_t address) {
		if (address > 99 || address < 0) return false;
		return true;
	}

	bool isValidValue(string value) {
		if (value.length() != 10 || value[0] != '0' || value[1] != 'x')
			return false;

		for (int i = 2; i < 10; i++) {
			if ((value[i] >= '0' && value[i] <= '9') || (value[i] >= 'A' && value[i] <= 'F')) continue;
			return false;
		}
		return true;
	}

	vector<string> splitBySpace(const string& input) {
		std::istringstream iss(input);
		std::string word;
		std::vector<string> result;

		while (iss >> word) {
			result.push_back(word);
		}
		return result;
	}

	bool isNumber(string s) {
		if (s.size() > 2) return false;

		for (auto c : s) {
			if (c < '0' || c > '9')
				return false;
		}
		return true;
	}

	bool isTestScript(string s) {
		if (s == "1_" || s == "2_" || s == "3_") {
			return true;
		}

		if (s == "1_FullWriteAndReadCompare"
			|| s == "2_PartialLBAWrite"
			|| s == "3_WriteReadAging")
			return true;

		return false;
	}
};