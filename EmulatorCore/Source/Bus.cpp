#include "Bus.h"

Bus::Bus(NesEmulator& emulator, CPU& cpu, PPU& ppu, IO& io, APU& apu, MMC* mmc)
	: emulator(emulator), cpu(cpu), ppu(ppu), io(io), apu(apu), mmc(mmc) {}

void Bus::Reset() {
	mmc->Reset();
	cpu.Reset();
	ppu.Reset();
	io.Reset();
	apu.Reset();
}

uint8_t Bus::Read(uint16_t address, bool fetch) {
	if (address < 0x2000) {
		return mmc->ReadRAM(address, fetch);
	}
	else if (address < 0x4000) {
		return ppu.ReadRegister(address % 0x8, fetch);
	}
	else if (address < 0x4020) {
		if ((address >= 0x4000 && address < 0x4014) || address == 0x4015 || address == 0x4017) {
			return apu.Read(address, fetch);
		}
		else {
			return io.ReadRegister(address - 0x4000, fetch);
		}
	}
	return mmc->Read(address, fetch);
}

void Bus::Write(uint16_t address, uint8_t value) {
	if (address < 0x2000) {
		mmc->WriteRAM(address, value);
		return;
	}
	else if (address < 0x4000) {
		ppu.WriteRegister((address - 0x2000) % 0x8, value);
		return;
	}
	else if (address < 0x4020) {
		if ((address >= 0x4000 && address < 0x4014) || address == 0x4015 || address == 0x4017) {
			apu.Write(address, value);
		}
		else {
			io.WriteRegister(address - 0x4000, value);
		}
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

const OAMEntry& Bus::GetOAMEntry(uint8_t address) {
	return mmc->GetOAMEntry(address);
}

OAMEntry* Bus::GetOAMBuffer() {
	return mmc->GetOAMBuffer();
}

void Bus::TriggerNMI() {
	cpu.NMIException();
}