#include "NESEmulator.h"

NesEmulator::NesEmulator() {
	bus = new Bus(this);
	mmc = new NMAP(bus);
	bus->mmc = mmc;
	io = new IO(bus);
	bus->io = io;
	apu = new APU(bus);
	bus->apu = apu;
	ppu = new PPU(bus);
	bus->ppu = ppu;
	cpu = new CPU(bus);
	bus->cpu = cpu;
}

NesEmulator::~NesEmulator() {
	delete bus;
	delete cpu;
	delete mmc;
	delete ppu;
	delete io;
	delete apu;
}

void NesEmulator::Reset() {
	bus->Reset();
}

void NesEmulator::Run(int32_t instructionsCount) {
	if (!romLoaded) return;
	for (int32_t i = 0; i < instructionsCount; i++) {
		uint32_t cpuClocks = cpu->Step();
		ppu->Step(cpuClocks);
		apu->Step(cpuClocks);
	}
}

bool NesEmulator::LoadROM(const uint8_t* data, const uint32_t size) {
	bus->romInfo = ROMHeader(size, data);
	bus->romInfo.Print();

	delete mmc;
	mmc = CreateMapper(data + 0x10);
	bus->mmc = mmc;

	Reset();

	romLoaded = true;
	return romLoaded;
}

MMC* NesEmulator::CreateMapper(const uint8_t* data) {
	switch (bus->romInfo.mapper) {
	case 1:
		return new MMC1(bus, bus->romInfo, data);
	case 0:
		return new NMAP(bus, bus->romInfo, data);
	default:
		return new NMAP(bus);
	}
}