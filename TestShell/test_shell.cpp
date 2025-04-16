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

	void write(uint32_t address, uint32_t value) {
		ssd->write(address, value);
	}
private:
	SSD* ssd;
};