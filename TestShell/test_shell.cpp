#include "interface.h"
#include "gmock/gmock.h"

class TestShell {
public:
	TestShell(SSD* ssd) : ssd(ssd) {}

	string read(uint32_t address) {
		if (!isValidAddress(address)) return "ERROR";
		return ssd->read(address);
	}

	string write(uint32_t address, string value) {
		if (!isValidAddress(address)) return "ERROR";
		if (!isValidValue(value)) return "ERROR";

		ssd->write(address, value);
		return "Done";
	}
private:
	SSD* ssd;

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
};