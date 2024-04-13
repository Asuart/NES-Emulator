#include "Bus.h"

Bus::Bus(NesEmulator* emulator)
	: emulator(emulator) {}

void Bus::Reset() {
	cpu->Reset();
	ppu->Reset();
	io->Reset();
	mmc->Reset();
}

bool Bus::LoadROM(const uint8_t* data, uint32_t size) {
	romInfo = ROMHeader(size, data);
	romInfo.Print();
	bool result = mmc->LoadROM(data + 16);
	Reset();
	return result;
}

uint8_t Bus::Read(uint16_t address, bool fetch) {
	if (address < 0x2000) {
		return mmc->ReadRAM(address, fetch);
	}
	else if (address < 0x4000) {
		return ppu->ReadRegister(address % 0x8, fetch);
	}
	else if (address < 0x4020) {
		return io->ReadRegister(address - 0x4000, fetch);
	}
	return mmc->Read(address, fetch);
}

void Bus::Write(uint16_t address, uint8_t value) {
	if (address < 0x2000) {
		mmc->WriteRAM(address, value);
		return;
	}
	else if (address < 0x4000) {
		ppu->WriteRegister((address - 0x2000) % 0x8, value);
		return;
	}
	else if (address < 0x4020) {
		io->WriteRegister(address - 0x4000, value);
		return;
	}
	mmc->Write(address, value);
}

uint8_t Bus::ReadVRAM(uint16_t address) {
	return mmc->ReadVRAM(address);
}

void Bus::WriteVRAM(uint16_t address, uint8_t value) {
	mmc->WriteVRAM(address, value);
}

uint8_t Bus::ReadOAM(uint8_t address) {
	return mmc->ReadOAM(address);
}

void Bus::WriteOAM(uint8_t address, uint8_t value) {
	mmc->WriteOAM(address, value);
}

OAMEntry Bus::GetOAMEntry(uint8_t address) {
	return mmc->GetOAMEntry(address);
}

void Bus::TriggerNMI() {
	cpu->NMIException();
}