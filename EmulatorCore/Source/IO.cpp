#include "IO.h"

IO::IO(Bus* bus)
	: bus(bus) {
	Reset();
}

void IO::Reset() {
	for (int32_t i = 0; i < keyStates.size(); i++) {
		keyStates[i] = 0;
	}
	readKeyNum = 0;
}

uint8_t IO::ReadRegister(uint8_t address, bool fetch) {
	uint8_t returnValue = registers[address];

	if (!fetch) {
		if (address == 0x16) {
			registers[address] = GetKeyState(readKeyNum++);
		}
	}

	return returnValue;
}

void IO::WriteRegister(uint8_t address, uint8_t value) {
	if (address == 0x14) {
		uint16_t romAddress = ((uint16_t)value << 8) & 0xff00;
		for (int32_t oamAddress = 0; oamAddress < 0x100; oamAddress++, romAddress++) {
			bus->WriteOAM(oamAddress, bus->Read(romAddress, true));
		}
	}
	else if (address == 0x16) {
		readKeyNum = 0;
		value = GetKeyState(readKeyNum++);
	}
	registers[address] = value;
}

uint8_t IO::GetKeyState(uint8_t key) {
	if (key >= keyStates.size()) return 1;
	return keyStates[key];
}