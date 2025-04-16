#include "interface.h"
#include "gmock/gmock.h"

class TestShell {
public:
	TestShell(SSD* ssd) : ssd(ssd) {}

	string read(uint32_t address) {
		if (address > 99 || address < 0)
			return "ERROR";
		return ssd->read(address);
	}

	string write(uint32_t address, string value) {
		if (address > 99 || address < 0)
			return "ERROR";

		if (value.length() == 10 && value[0] == '0' && value[1] == 'x') {
			for (int i = 2; i < 10; i++) {
				if ((value[i] >= '0' && value[i] <= '9') || (value[i] >= 'A' && value[i] <= 'F')) continue;

				return "ERROR";
			}
		}
		else {
			return "ERROR";
		}

		ssd->write(address, value);
		return "Done";
	}
private:
	SSD* ssd;
};